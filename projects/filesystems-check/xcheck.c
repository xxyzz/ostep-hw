#include "fs.h"
#include "param.h"
#include "stat.h"
#include <errno.h>
#include <math.h> // fmin
#include <stdbool.h>
#include <stdio.h>    // fopen, fclose, fileno
#include <stdlib.h>   // exit
#include <string.h>   // memmove, strcmp, memset
#include <sys/mman.h> // mmap, munmap

#define IMG_SIZE (FSSIZE * BSIZE)
#define NBMAP FSSIZE / 8

void read_inode_data(struct dinode inode, void *imgp, void *destp, int offset,
                     int size) {
  uint block_num = offset / BSIZE;
  uint addr = 0;
  if (block_num < NDIRECT)
    addr = inode.addrs[block_num];
  else {
    block_num -= NDIRECT;
    if (block_num < NINDIRECT) {
      uint indirect[NINDIRECT];
      memmove(indirect, imgp + inode.addrs[NDIRECT] * BSIZE, sizeof(indirect));
      addr = indirect[block_num];
    }
  }
  memmove(destp, imgp + addr * BSIZE + offset % BSIZE, BSIZE);
}

void check_inode_type(int type) {
  if (type != 0 && type != T_DIR && type != T_FILE && type != T_DEV) {
    fprintf(stderr, "ERROR: bad inode.\n");
    exit(EXIT_FAILURE);
  }
}

void check_address(uint addr, bool direct, uint data_start, uchar *bmap,
                   uchar bmap_mark[]) {
  if ((addr < data_start && addr != 0) || addr >= FSSIZE) {
    if (direct)
      fprintf(stderr, "ERROR: bad direct address in inode.\n");
    else
      fprintf(stderr, "ERROR: bad indirect address in inode.\n");
    exit(EXIT_FAILURE);
  }

  if (addr == 0)
    return;
  // error 5
  uint index = addr - data_start;
  uint b = 0x1 << (index % 8);
  if (!(bmap[index / 8] & b)) {
    fprintf(stderr,
            "ERROR: address used by inode but marked free in bitmap.\n");
    exit(EXIT_FAILURE);
  }
  if (bmap_mark[index / 8] & b) { // error 6 & 7
    if (direct)
      fprintf(stderr, "ERROR: direct address used more than once.\n");
    else
      fprintf(stderr, "ERROR: indirect address used more than once.\n");
    exit(EXIT_FAILURE);
  }
  bmap_mark[index / 8] |= b;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: xcheck <file_system_image>\n");
    exit(EXIT_FAILURE);
  }
  FILE *img_file = fopen(argv[1], "r");
  if (img_file == NULL) {
    if (errno == ENOENT)
      fprintf(stderr, "image not found.\n");
    else
      fprintf(stderr, "fopen failed\n");
    exit(EXIT_FAILURE);
  }
  int fd = fileno(img_file);
  if (fd == -1) {
    fprintf(stderr, "fileno failed\n");
    exit(EXIT_FAILURE);
  }
  void *imgp = mmap(NULL, IMG_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
  if (imgp == NULL) {
    fprintf(stderr, "mmap failed\n");
    exit(EXIT_FAILURE);
  }
  if (fclose(img_file) == EOF) {
    fprintf(stderr, "fclose failed\n");
    exit(EXIT_FAILURE);
  }
  struct superblock sb;
  memmove(&sb, imgp + BSIZE, sizeof(sb));
  int data_start = FSSIZE - sb.nblocks;
  struct dinode inodes[sb.ninodes];
  memmove(inodes, imgp + sb.inodestart * BSIZE, sizeof(inodes));
  uchar bmap[NBMAP];
  uchar bmap_mark[NBMAP] = {0};
  uchar inode_dir[sb.ninodes];
  uint dir_links[sb.ninodes];
  memset(inode_dir, 0, sizeof(inode_dir));
  memset(dir_links, 0, sizeof(dir_links));
  memmove(bmap, imgp + sb.bmapstart * BSIZE, sizeof(bmap));
  for (int i = ROOTINO; i < sb.ninodes; i++) {
    struct dinode inode = inodes[i];
    check_inode_type(inode.type); // error 1

    // check inode address range, error 2
    if (inode.type != 0) {
      for (int j = 0; j < NDIRECT + 1; j++) {
        if (j < NDIRECT)
          check_address(inode.addrs[j], true, data_start, bmap, bmap_mark);
        else {
          uint indirect[NINDIRECT];
          check_address(inode.addrs[NDIRECT], false, data_start, bmap,
                        bmap_mark);
          memmove(indirect, imgp + inode.addrs[NDIRECT] * BSIZE,
                  sizeof(indirect));
          for (int k = 0; k < NINDIRECT; k++)
            check_address(indirect[k], false, data_start, bmap, bmap_mark);
        }
      }
    }

    // check dir, error 3 & 4
    if (inode.type == T_DIR) {
      int count_dots = 0;
      bool root_exist = false;
      bool dir_error = false;
      struct dirent de;
      for (int off = 0; off < inode.size; off += sizeof(de)) {
        read_inode_data(inode, imgp, &de, off, sizeof(de));
        if (strcmp(de.name, ".") == 0) {
          count_dots++;
          if (de.inum != i) {
            dir_error = true;
            break;
          }
        } else if (strcmp(de.name, "..") == 0) {
          count_dots++;
          if (i == ROOTINO && de.inum == ROOTINO)
            root_exist = true;
        }
        inode_dir[de.inum] = 1;
        if (inodes[de.inum].type == T_FILE)
          inodes[de.inum].nlink--;
        else if (inodes[de.inum].type == T_DIR && i != de.inum)
          dir_links[de.inum]++;
      }
      if (i == ROOTINO && !root_exist) {
        fprintf(stderr, "ERROR: root directory does not exist.\n");
        exit(EXIT_FAILURE);
      }
      if (dir_error || count_dots != 2) {
        fprintf(stderr, "ERROR: directory not properly formatted.\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  // error 6
  for (int j = 0; j < NBMAP; j += 8) {
    uint a = bmap[j / 8];
    uint b = bmap_mark[j / 8];
    if (a ^ b) {
      fprintf(stderr,
              "ERROR: bitmap marks block in use but it is not in use.\n");
      exit(EXIT_FAILURE);
    }
  }

  for (int j = ROOTINO; j < sb.ninodes; j++) {
    if (inodes[j].type != 0 && inode_dir[j] == 0) { // error 9
      fprintf(stderr,
              "ERROR: inode marked use but not found in a directory.\n");
      exit(EXIT_FAILURE);
    }
    if (inode_dir[j] == 1 && inodes[j].type == 0) { // error 10
      fprintf(stderr,
              "ERROR: inode referred to in directory but marked free.\n");
      exit(EXIT_FAILURE);
    }
    if (inodes[j].type == T_FILE && inodes[j].nlink > 0) { // error 11
      fprintf(stderr, "ERROR: bad reference count for file.\n");
      exit(EXIT_FAILURE);
    }
    if (inodes[j].type == T_DIR && dir_links[j] > 1) { // error 12
      fprintf(stderr,
              "ERROR: directory appears more than once in file system.\n");
      exit(EXIT_FAILURE);
    }
  }

  if (munmap(imgp, IMG_SIZE) == -1) {
    fprintf(stderr, "munmap failed\n");
    exit(EXIT_FAILURE);
  }
}
