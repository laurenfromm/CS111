//NAME: Lauren Fromm
//EMAIL: laurenfromm@gmail.com
//ID: 404751250


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include <math.h>
#include "ext2_fs.h"

char* file;
int fd;
struct ext2_super_block  superblock;
struct ext2_group_desc  *groups;
int numGroups;


void superblockSummary()
{

  if(pread(fd, &superblock, sizeof(superblock), 1024) == -1)
    {
      fprintf(stderr, "Error getting superblock infromation");
      exit(1);
    }
  numGroups = 1+ superblock.s_blocks_count / superblock.s_blocks_per_group;

  fprintf(stdout, "SUPERBLOCK,%u,%u,%u,%u,%u,%u,%u\n", superblock.s_blocks_count, superblock.s_inodes_count, 1024 << superblock.s_log_block_size, superblock.s_inode_size, superblock.s_blocks_per_group, superblock.s_inodes_per_group, superblock.s_first_ino);

}

void groupSummary(){


  groups = malloc(numGroups * sizeof(struct ext2_group_desc));
  int i = 0;
  int sblocksize = 1024 << superblock.s_log_block_size;
  for(; i < numGroups; i++)
    {

      if(pread(fd, groups, numGroups * (sizeof(struct ext2_group_desc)), 1024 + sblocksize +  (i *  (sizeof(struct ext2_group_desc)))) == -1) 
	{
	  fprintf(stderr, "Error reading group information");
	  exit(1);
	}

       	

	fprintf(stdout, "GROUP,%d,%u,%u,%u,%u,%u,%u,%u\n", i, superblock.s_blocks_count, superblock.s_inodes_count, groups[i].bg_free_blocks_count, groups[i].bg_free_inodes_count, groups[i].bg_block_bitmap, groups[i].bg_inode_bitmap, groups[i].bg_inode_table);
    }


}


void freeBlock(){

  int i = 0;
  int sblocksize = 1024 << superblock.s_log_block_size;
  for(;i != numGroups; i++)
    {
      int j = 0;
      for (; j < sblocksize; j++)
	{
	  
	  uint8_t byte;
	  if(pread(fd, &byte, 1, (groups[i].bg_block_bitmap * sblocksize) + j) == -1)
	    {
	      fprintf(stderr, "Error reading bitmap\n");
	      exit(1);
	    }
	  int k = 0;
	  unsigned long mask = 1;
	  for( ; k < 8; k++)
	    {
	      if((byte & mask ) ==0)
		fprintf(stdout, "BFREE,%u\n", (i*superblock.s_blocks_per_group) + (8*j)+ k +1 );
	      mask = mask << 1;
	    }
	}
    }



}



void freeInode(){
  int i = 0;
  int sblocksize = 1024 << superblock.s_log_block_size;
  for(; i < numGroups; i++)
    {

      int j = 0;
      for (; j < sblocksize; j++)
	{
	  uint8_t byte;
	  if(pread(fd, &byte, 1, (groups[i].bg_inode_bitmap * sblocksize) + j) == -1)
	    {
	      fprintf(stderr, "Error reading inode bitmap\n");
	      exit(1);
	    }
	  int k = 0;
	  unsigned long mask =1;
	  for (; k < 8; k ++)
	    {
	      if((byte & mask) == 0)
		fprintf(stdout, "IFREE,%u\n", (i*superblock.s_inodes_per_group) + ( 8* j) +k +1);
	      mask = mask << 1;
	    }
	}

    }



}

void directoryEntries(struct ext2_inode *inode, int num){
  //fprintf(stderr, "in direct entries for num : %d\n", num);
  unsigned int offset = 0;
  
  struct ext2_dir_entry direct;
  direct.file_type = 1;
  int i = 0;
  int sblocksize= 1024 << superblock.s_log_block_size;
  for(; i < 12; i++)
    {
      offset = 0;
      //      if(num == 2){
      //	fprintf(stderr, "tester\n");
      //	fprintf(stderr, "offset : %d\n inode->size: %u\n direct.file_type: %d\n",offset, inode->i_size, direct.file_type);
      //}
      while(offset < inode->i_size && direct.file_type != 0)
	{
	  //  if(num==2)
	  //  fprintf(stderr,"test\n");
	  if(pread(fd, &direct, sizeof(struct ext2_dir_entry), (inode->i_block[i] * sblocksize) + offset) == -1)
	    {
	      fprintf(stderr, "Error reading directory entries\n");
	      exit(1);
	    }
	 
	  if(direct.inode != 0x0000)
	    fprintf(stdout, "DIRENT,%d,%u,%u,%u,%u,'%s'\n", num, offset, direct.inode, direct.rec_len, direct.name_len,direct.name);
	
	  offset+= direct.rec_len;
	}


    }


}


void indirectBlocks(int offset, int block, int flag ,int num)
{
  unsigned int sblocksize = 1024 << superblock.s_log_block_size;
  int* arr =  malloc(sblocksize);
  if(pread(fd, arr, sblocksize,  block * sblocksize) == -1)
    {
      fprintf(stderr, "Error getting indirect blocks\n");
      exit(1);
    }
  int i = 0;
  //  int limit = sblocksize / 4;
  //unsigned int offset = 0;
  //int arr[limit];
  //memset(arr, 0, sizeof(arr));
  //if(pread(fd, arr, sblocksize, 
  if (flag == 12)
    {
   //      offset = 12;
      int j = 0;
      for(; j < 256; j++)
	{
	 
	  if(arr[j] != 0)
	    {
	      fprintf(stdout, "INDIRECT,%u,%u,%u,%u,%u\n", num, flag-11, offset+j, block, arr[j]);
	    }

	}
    }
	  
  if (flag == 13 || flag == 14)
    {
      int k = 0;
      for (; k < 256; k++)
	{
	  if(arr[k] != 0) {
	    fprintf(stdout, "INDIRECT,%u,%u,%u,%u,%u\n", num, flag-11, offset+k, block, arr[k]);
	    indirectBlocks(offset + i, arr[k], flag-1, num);
	  }
	  
	}

    }
 
}




void inodeSummary(){

  
  int i = 0;
  int sblocksize  = 1024 << superblock.s_log_block_size;
  for(; i != numGroups; i++)
    {
      unsigned int j = 0;
      for(; j < superblock.s_inodes_count; j++)
	{
	  struct ext2_inode temp;
	  if(pread(fd, &temp, sizeof(struct ext2_inode), (1024 + (4 * sblocksize) + (j * sizeof(struct ext2_inode)))) == -1 )
	    {
	      fprintf(stderr, "Error getting inodes");
	      exit(1);
	    }
	  if( temp.i_mode == 0x0000 || temp.i_links_count == 0x0000)
	    continue;
	  else
	    {
	      char fileType = '?';
	      if(temp.i_mode & 0xA000)
		fileType = 's';
	      if(temp.i_mode & 0x8000)
		fileType = 'f';
	      if(temp.i_mode & 0x4000)
		fileType = 'd';

	      int mode = temp.i_mode & (0x01C0 | 0x0038 | 0x007);


	      char ctime[80];
	      char mtime[80];
	      char atime[80];

	      time_t c = temp.i_ctime;
	      struct tm ts = *gmtime(&c);
	      strftime(ctime, 80, "%m/%d/%y %H:%M:%S", &ts);

	      time_t m = temp.i_mtime;
	      struct tm ts1 = *gmtime(&m);
	      strftime(mtime, 80, "%m/%d/%y %H:%M:%S", &ts1);

	      time_t a = temp.i_atime;
	      struct tm ts2 = *gmtime(&a);
	      strftime(atime, 80, "%m/%d/%y %H:%M:%S", &ts2);


	      fprintf(stdout, "INODE,%d,%c,%o,%u,%u,%u,%s,%s,%s,%u,%u", j+1, fileType, mode, temp.i_uid, temp.i_gid, temp.i_links_count, ctime, mtime, atime, temp.i_size, temp.i_blocks);
	      
	      struct ext2_inode temp2 = temp;
	      
	      int it = 0;
	      for(; it < 15; it++)
		{
	      	  fprintf(stdout,",%u", temp2.i_block[it]);
		  
	      	}
	      fprintf(stdout,"\n");

	      if(fileType == 'd')
		{
		  directoryEntries(&temp, j+1);
		}
	      if(temp.i_block[12] != 0)
		indirectBlocks(12, temp.i_block[12], 12, j+1);
	      if(temp.i_block[13] != 0)
		indirectBlocks(12 + 256, temp.i_block[13], 13, j+1);
	      if(temp.i_block[14] != 0)
		indirectBlocks(12 + 256 + 65536, temp.i_block[14], 14, j+1);
	    }
	  
	}

    }
}


int main(int argc, char* argv[])
{
  if (argc != 2)
    {
      fprintf(stderr, "Error: One argument required \n");
      exit(1);
    }
  int length = strlen(argv[1]) + 1;
  file = malloc(length * sizeof(char));
  file = argv[1];
  fd  = open(file, O_RDONLY);
  //  superblock = malloc(sizeof(struct ext2_super_block));
  

 

  superblockSummary();
  groupSummary();
  freeBlock();
  freeInode();
  inodeSummary();
  //  directoryEntries();
  //indirectBlocks();
  exit(0);
}
