#ifndef VFS_H
#define VFS_H

#include <common.h>

struct fs_node;

typedef uint32_t (*read_type_t)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef uint32_t (*write_type_t)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent* (*readdir_type_t)(struct fs_node*, uint32_t);
typedef struct fs_node* (*finddir_type_t)(struct fs_node*, char*);

// VFS node
typedef struct fs_node {
    char name[128];
    uint32_t mask;
    uint32_t uid;
    uint32_t gid;
    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    uint32_t impl;

    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;

    struct fs_node* ptr;
} fs_node_t;

// Directory entry
struct dirent {
    char name[128];
    uint32_t inode;
};

// FS node flags
enum node_flags {
    FS_FILE = 1,
    FS_DIRECTORY,
    FS_CHARDEVICE,
    FS_BLOCKDEVICE,
    FS_PIPE,
    FS_SYMLINK,
    FS_MOUNTPOINT = 8,
};

// Root node of the VFS
extern fs_node_t* vfs_root;

// TODO: replace `uint32_t` with `size_t` probably...

// Reads the contents of a node in the VFS.
uint32_t vfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);

// Writes to the contents of a node in the VFS.
uint32_t vfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);

// Opens a node in the VFS.
void vfs_open(fs_node_t* node, uint8_t read, uint8_t write);

// Closes a node in the VFS.
void vfs_close(fs_node_t* node);

// Reads a directory entry from the VFS.
struct dirent* vfs_readdir(fs_node_t* node, uint32_t index);

// Finds a directory node in the VFS.
fs_node_t* vfs_finddir(fs_node_t* node, char* name);

#endif