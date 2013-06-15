/* files.c
   aleph-avr32
  
   filesystem routines

   TODO: should move this to BEES and abstract the things that should be hidden (e.g. interrupt control)
*/

// std
#include <stdlib.h>
#include <string.h>
// asf
#include "compiler.h"
#include "print_funcs.h"

// aleph-avr32
#include "bfin.h"
#include "files.h"
#include "filesystem.h"
#include "flash.h"
#include "memory.h"
// boot
// #include "parse_hex.h"

// ---- directory list class
// params
#define DIR_LIST_MAX_NUM 64
#define DIR_LIST_NAME_LEN 64
#define DIR_LIST_NAME_BUF_SIZE 2048 // len * num

//  stupid datatype with fixed number of fixed-length filenames
// storing this for speed when UI asks us for a lot of strings
typedef struct _dirList {
  char path[8];
  volatile char nameBuf[DIR_LIST_NAME_BUF_SIZE];
  //  u32 size[DIR_LIST_MAX_NUM];
  u32 num;
} dirList_t;


//-----------------------------
//----  extern vars

//------------------------------
//----- -static vars


/// TODO: get these out of the core avr32 source somehow.
// directory lists;
static dirList_t dspList;
static dirList_t sceneList;

//----------------------------------
//---- static functions

// populate list with filenames and count
static void list_scan(dirList_t* list, const char* path);
// get name at idx
const char* list_get_name(dirList_t* list, u8 idx);
// get file pointer if found (caller must close)
//static void* list_open_file_name(dirList_t * list, const char* name, const char* mode);
// ugly, set size by pointer
static void* list_open_file_name(dirList_t* list, const char* name, const char* mode, u32* size);

//// FIXME: dumb and slow seek/read functions because the real ones are broken
//// fseek: no offset arg, assume its the first seek since file was opened 
/* static void fake_fseek(void* fp, u32 loc) { */
/*   u32 n = 0; */
/*   u8 dum; */
/*   while(n < loc) { */
/*     dum = fl_fgetc(fp); */
/*     n++; */
/*   }  */
/* } */
// fread: no size arg
static void fake_fread(volatile u8* dst, u32 size, void* fp) {
  u32 n = 0;
  while(n < size) {
    *dst = fl_fgetc(fp);
    n++;
    dst++;
  }
}

//---------------------------
//------------- extern defs

void init_files(void) {
  // init FAT lib
  fat_init();
  // scan directories
  list_scan(&dspList, "/dsp/");
  list_scan(&sceneList, "/scenes/");
}


//----- dsp management

// return filename for DSP given index in list
const volatile char* files_get_dsp_name(u8 idx) {
  return list_get_name(&dspList, idx);
}

// return filename for scene given index in list
const volatile char* files_get_scene_name(u8 idx) {
  return list_get_name(&sceneList, idx);
}


// load a blacfkin executable by index */
void files_load_dsp(u8 idx) {  
  files_load_dsp_name((const char*)files_get_dsp_name(idx));
}

// search for specified dsp file and load it
void files_load_dsp_name(const char* name) {
  void* fp;
  //  u32 bytesRead;
  u32 size = 0;

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);

  fp = list_open_file_name(&dspList, name, "r", &size);

  if( fp != NULL) {	  
    print_dbg("\r\n found file, loading dsp ");
    print_dbg(name);
    /// FIXME:
    /// arrg, why is fl_fread intermittently broken?
    /// check our media access functions against fat_filelib.c, i guess
    //    bytesRead = fl_fread((void*)bfinLdrData, 1, size, fp);

    print_dbg("\r\n bfinLdrData : @0x");
    print_dbg_hex( (u32)bfinLdrData );


    fake_fread(bfinLdrData, size, fp);
    // print_dbg("\r\n finished fakefread");
    fl_fclose(fp);
    bfinLdrSize = size;
    print_dbg("\r\n loading bfin from buf");
    bfin_load_buf();
    print_dbg("\r\n finished load");
  } else {
    print_dbg("\r\n error: fp was null in files_load_dsp_name \r\n");
  }


  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);
}


// store .ldr as default in internal flash
void files_store_default_dsp(u8 idx) {
  const char* name;
  void* fp;	  
  u32 size;

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);

  name = (const char*)files_get_dsp_name(idx);
  fp = list_open_file_name(&dspList, name, "r", &size);

  if( fp != NULL) {
    print_dbg("\r\n writing (this may take a while)...");
    bfinLdrSize = size;
    fl_fread((void*)bfinLdrData, 1, size, fp);
    flash_write_ldr();
    fl_fclose(fp);
    print_dbg("finished writing LDR to flash");
    
  } else {
    print_dbg("\r\n error: fp was null in files_store_default_dsp \r\n");
  }

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);
}

// return count of dsp files
u8 files_get_dsp_count(void) {
  return dspList.num;
}

//----- scenes management
// return filename for scene given index in list

// load scene by index */
void files_load_scene(u8 idx) {  
  files_load_scene_name((const char*)files_get_scene_name(idx));
}

// search for specified scene file and load it
void files_load_scene_name(const char* name) {
  void* fp;
  //  u32 bytesRead;
  u32 size = 0;

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);

  fp = list_open_file_name(&dspList, name, "r", &size);

  if( fp != NULL) {	  
    /////////
    /////////
    /// put this in
    /////////
    /////////////


    /* print_dbg("\r\n found file, loading dsp "); */
    /* print_dbg(name); */
    /* /// FIXME: */
    /* /// arrg, why is fl_fread intermittently broken? */
    /* /// check our media access functions against fat_filelib.c, i guess */
    /* //    bytesRead = fl_fread((void*)bfinLdrData, 1, size, fp); */
    /* fake_fread(bfinLdrData, size, fp); */
    /* fl_fclose(fp); */
    /* bfinLdrSize = size; */
    /* bfin_load_buf(); */

  } else {
    print_dbg("\r\n error: fp was null in files_load_scene_name \r\n");
  }
 
  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);
}


  // store scene to sdcard at idx
void files_store_scene(u8 idx) {
  // fill the scene RAM buffer from current state of system
  //  scene_write_buf();
  // write it to sdcard

}
  // store scene to sdcard at name
// void files_store_scene_name(const char* name) {
  //  scene_write_buf();
// }



// store scene as default in internal flash
void files_store_default_scene(u8 idx) {
  const char* name;
  void* fp;	  
  u32 size;

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);

  name = (const char*)files_get_scene_name(idx);
  fp = list_open_file_name(&dspList, name, "r", &size);

  if( fp != NULL) {
    print_dbg("\r\n writing (this may take a while)...");
    //    bfinLdrSize = size;
    //    fl_fread((void*)bfinLdrData, 1, size, fp);
    //    flash_write_ldr();
    fl_fclose(fp);
    print_dbg("finished storing default scene");
    
  } else {
    print_dbg("\r\n error: fp was null in files_store_default_scene \r\n");
  }

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);
}

// return count of dsp files
u8 files_get_scene_count(void) {
  return dspList.num;
}



//---------------------
//------ static

const char* list_get_name(dirList_t* list, u8 idx) {
  return (const char*) ( list->nameBuf + (idx * DIR_LIST_NAME_LEN) );
}

void list_scan(dirList_t* list, const char* path) {
  FL_DIR dirstat;
  struct fs_dir_ent dirent;
  list->num = 0;
  strcpy(list->path, path);
  if( fl_opendir(path, &dirstat) ) {      
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if( !(dirent.is_dir) ) {
	strcpy((char*)list->nameBuf + (list->num * DIR_LIST_NAME_LEN), dirent.filename);
	list->num++;
      }
    }
  }
}

// search for a given filename in a listed directory. set size by pointer
void* list_open_file_name(dirList_t* list, const char* name, const char* mode, u32* size) {

  FL_DIR dirstat;
  struct fs_dir_ent dirent;
  char path[64];
  void* fp;

  //  name = list_get_name(lista, idx);
  strcpy(path, list->path);

  print_dbg("\r\n attempting to open file at path: \r\n");
  print_dbg(path);

  if(fl_opendir(path, &dirstat)) {
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if (strcmp(dirent.filename, name) == 0) {
	strncat(path, dirent.filename, 58);
	
	print_dbg("\r\n attempting to open file at path: \r\n");
	print_dbg(path);
	
	print_dbg("\r\n name: \r\n");
	print_dbg(path);

	fp = fl_fopen(path, mode);
	*size = dirent.size;
	break;
      } else { // no match
	*size = 0;
	fp = NULL;
      }
    } // end loop
  } else { // dir error
    *size = 0;
    fp = NULL;
  }
  return fp;
}

/* //// /test: write dummy file */
/* fp = fl_fopen("/tetblsts.txt", "w"); */
/* print_dbg("\r\n dummy write test, fp: "); */
/* print_dbg_hex(fp); */
/* fl_fputs("hihhihi", fp); */
/* fl_fclose(fp); */



