#include <vfs.h>

fs_node_t* vfs_root = NULL;

uint32_t vfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node->read != 0) {
        return node->read(node, offset, size, buffer);
    }

    return 0;
}

uint32_t vfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node->write != 0) {
        return node->write(node, offset, size, buffer);
    }

    return 0;
}

void vfs_open(fs_node_t* node, uint8_t read, uint8_t write) {
    if (node->open != 0) {
        node->open(node);
    }

    // not sure what to do with read and write..
    (void)read;
    (void)write;
}

void vfs_close(fs_node_t* node) {
    if (node->close != 0) {
        node->close(node);
    }
}

struct dirent* vfs_readdir(fs_node_t* node, uint32_t index) {
    if ((node->flags & 0x7) == FS_DIRECTORY && node->readdir != 0) {
        return node->readdir(node, index);
    }

    return NULL;
}

fs_node_t* vfs_finddir(fs_node_t* node, char* name) {
    if ((node->flags & 0x7) == FS_DIRECTORY && node->finddir != 0) {
        return node->finddir(node, name);
    }

    return NULL;
}