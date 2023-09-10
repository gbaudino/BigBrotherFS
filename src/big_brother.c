#include "big_brother.h"
#include "fat_volume.h"
#include "fat_table.h"
#include "fat_util.h"
#include <stdio.h>
#include <gmodule.h>
#include <string.h>

char *private_words[] = {"Oldspeak", "English",   "revolution",
                         "Emmanuel", "Goldstein", NULL};

GSList *private_words_found(const char *buf) {
    GSList *words_found = NULL;
    for (unsigned int i = 0; private_words[i] != NULL; i++) {
        if (strstr(buf, private_words[i]) != NULL) {
            DEBUG("The string %s was found in the buf", private_words[i]);
            words_found = g_slist_prepend(words_found, private_words[i]);
        }
    }
    return words_found;
}

int bb_is_log_file_dentry(fat_dir_entry dir_entry) {
    return strncmp(LOG_FILE_BASENAME, (char *)(dir_entry->base_name), 3) == 0 &&
           strncmp(LOG_FILE_EXTENSION, (char *)(dir_entry->extension), 3) == 0;
}

int bb_is_log_filepath(char *filepath) {
    return strncmp(BB_LOG_FILE, filepath, 8) == 0;
}

int bb_is_log_dirpath(char *filepath) {
    return strncmp(BB_DIRNAME, filepath, 15) == 0;
}

static bool is_cluster_bad(u32 cluster) {
    fat_volume vol = get_fat_volume();
    return fat_table_cluster_is_bad_sector((u32)le32_to_cpu(((const le32 *)vol->table->fat_map)[cluster]));
}

/* Searches for a cluster that could correspond to the bb directory and returns
 * its index. If the cluster is not found, returns 0.
 */
u32 search_bb_orphan_dir_cluster() {
    u32 bb_dir_start_cluster = 2;
    u32 max_clusters = 10000;

    fat_volume vol = get_fat_volume();
    u32 bytes_per_cluster = fat_table_bytes_per_cluster(vol->table);
    while (bb_dir_start_cluster < max_clusters) {
        if (!is_cluster_bad(bb_dir_start_cluster)) {
            DEBUG("EL CLUSTER %d NO ES MALO", bb_dir_start_cluster);
            bb_dir_start_cluster++;
            continue;
        }
        DEBUG("EL CLUSTER %d ES MALO", bb_dir_start_cluster);
        off_t offset = fat_table_cluster_offset(vol->table, bb_dir_start_cluster);
        u8 *buf = alloca(bytes_per_cluster);
        full_pread(vol->table->fd, buf, bytes_per_cluster, offset);       
        fat_dir_entry dentry = (fat_dir_entry) buf;

        if(bb_is_log_file_dentry(dentry)) {
            DEBUG("NOMBRE ENCONTRADO: %s.%s",dentry->base_name, dentry->extension);
            return bb_dir_start_cluster;
        }
        DEBUG("NOMBRE NO ENCONTRADO EN CLUSTER %u. SE CONTINUA BUSQUEDA", bb_dir_start_cluster);
        bb_dir_start_cluster++;
    }
    DEBUG("CLUSTER NO ENCONTRADO");
    return 0;
}

int bb_init_log_dir(u32 start_cluster) {
    errno = 0;

    fat_volume vol = NULL;
    vol = get_fat_volume();
    bool need_insert_children = false;
    
    fat_tree_node root_node = fat_tree_node_search(vol->file_tree, "/");

    if (start_cluster == 0) {
        // create a new orphan directory
        start_cluster = fat_table_get_next_free_cluster(vol->table);
        DEBUG("CREANDO NUEVO DIRECTORIO HUERFANO EN CLUSTER %u", start_cluster);
    } else {
        DEBUG("FLACO YA EXISTE EL DIRECTORIO");
        need_insert_children = true;
    }
    
   
    fat_file bb_dir = fat_file_init_orphan_dir(BB_DIRNAME, vol->table, start_cluster);
    fat_table_set_next_cluster(vol->table, start_cluster, FAT_CLUSTER_BAD_SECTOR);

    // add the bb dir to the tree
    fat_tree_insert(vol->file_tree, root_node, bb_dir);

    if (need_insert_children){
        fat_tree_node bb_node = fat_tree_node_search(vol->file_tree, BB_DIRNAME);
        GList *children = fat_file_read_children(bb_dir);
        if (children != NULL) {
            DEBUG("EL DIRECTORIO YA TIENE ARCHIVOS");
            fat_tree_insert(vol->file_tree, bb_node, g_list_nth_data(children, 0));
        }
    }
    
    return -errno;
}
