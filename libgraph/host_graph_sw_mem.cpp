#include "host_graph_sw.h"

#include "he_mem_config.h"


const int sizeAttrLut[] = {ATTR_PL_DDR3, ATTR_PL_DDR2, ATTR_PL_DDR1, ATTR_PL_DDR0};

const int cuAttrLut[] = {ATTR_PL_DDR3, ATTR_PL_DDR2, ATTR_PL_DDR1, ATTR_PL_DDR0};


#define PARTITION_DDR       (sizeAttrLut[cuIndex])
#define CU_DDR              (cuAttrLut[cuIndex])

void base_mem_init(cl_context &context)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(local_mem); i++)
    {
        he_mem_init(context, &local_mem[i]);
    }
}

static void gs_mem_init(cl_context &context, gatherScatterDescriptor *gsItem, int cuIndex, void *data)
{
    gsItem->prop[0].id =  MEM_ID_GS_BASE + cuIndex * MEM_ID_GS_OFFSET;
    gsItem->prop[0].name = "cu prop ping";
    gsItem->prop[0].attr = CU_DDR;
    gsItem->prop[0].unit_size = sizeof(int);
    gsItem->prop[0].size_attr = SIZE_IN_VERTEX;
    he_mem_init(context, &gsItem->prop[0]);
    memcpy(gsItem->prop[0].data, data, gsItem->prop[0].size);

    gsItem->prop[1].id =  MEM_ID_GS_BASE + cuIndex * MEM_ID_GS_OFFSET + 2;
    gsItem->prop[1].name = "cu prop pong";
    gsItem->prop[1].attr = CU_DDR;
    gsItem->prop[1].unit_size = sizeof(int);
    gsItem->prop[1].size_attr = SIZE_IN_VERTEX;
    he_mem_init(context, &gsItem->prop[1]);
    memcpy(gsItem->prop[1].data, data, gsItem->prop[1].size);

    gsItem->tmpProp.id =  MEM_ID_GS_BASE + cuIndex * MEM_ID_GS_OFFSET + 1;
    gsItem->tmpProp.name = "cu output tmpProp";
    gsItem->tmpProp.attr = CU_DDR;
    gsItem->tmpProp.unit_size = sizeof(int);
    gsItem->tmpProp.size_attr = SIZE_IN_VERTEX;
    he_mem_init(context, &gsItem->tmpProp);
}


void process_mem_init(cl_context &context)
{
    int *vertexScoreMapped =       (int*)get_host_mem_pointer(MEM_ID_VERTEX_SCORE_MAPPED);
    for (int i = 0; i < SUB_PARTITION_NUM; i++)
    {
        gs_mem_init(context, getGatherScatter(i), i, vertexScoreMapped);
    }
}

void partition_mem_init(cl_context &context, int blkIndex, int size, int cuIndex)
{
    int i = blkIndex;
    subPartitionDescriptor *partitionItem  = getSubPartition(i);
    {
        partitionItem->cuIndex = cuIndex;
        partitionItem->edge.id = MEM_ID_PARTITION_BASE + i * MEM_ID_PARTITION_OFFSET;
        partitionItem->edge.name = "partition edge";
        partitionItem->edge.attr = PARTITION_DDR;
        partitionItem->edge.unit_size = size * sizeof(int);
        partitionItem->edge.size_attr = SIZE_USER_DEFINE;
        he_mem_init(context, &partitionItem->edge);

        partitionItem->edgeMap.id = MEM_ID_PARTITION_BASE + i * MEM_ID_PARTITION_OFFSET + 1;
        partitionItem->edgeMap.name = "partition edgeMap";
        partitionItem->edgeMap.attr = PARTITION_DDR;
        partitionItem->edgeMap.unit_size = size * sizeof(int);
        partitionItem->edgeMap.size_attr = SIZE_USER_DEFINE;
        he_mem_init(context, &partitionItem->edgeMap);

        partitionItem->edgeProp.id = MEM_ID_PARTITION_BASE + i * MEM_ID_PARTITION_OFFSET + 2;
        partitionItem->edgeProp.name = "partition edgeProp";
#if 1
        partitionItem->edgeProp.attr = PARTITION_DDR;
#else
        partitionItem->edgeProp.attr = ATTR_HOST_ONLY;
#endif
        partitionItem->edgeProp.unit_size = size * sizeof(int);
        partitionItem->edgeProp.size_attr = SIZE_USER_DEFINE;
        he_mem_init(context, &partitionItem->edgeProp);

        partitionItem->tmpProp.id = MEM_ID_PARTITION_BASE + i * MEM_ID_PARTITION_OFFSET + 3;
        partitionItem->tmpProp.name = "partition tmpProp";
        partitionItem->tmpProp.attr = PARTITION_DDR;
        partitionItem->tmpProp.unit_size = VERTEX_MAX * sizeof(int);
        partitionItem->tmpProp.size_attr = SIZE_USER_DEFINE;
        he_mem_init(context, &partitionItem->tmpProp);
    }
}
