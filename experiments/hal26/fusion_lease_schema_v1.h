#ifndef HAL26_FUSION_LEASE_SCHEMA
#define HAL26_FUSION_LEASE_SCHEMA
#include <stdint.h>
/* Scope is ONLY the existing origin leases. Never mark complete owner coverage. */
typedef struct {uint16_t deck,block;uint32_t card,serial;} FlRow;
typedef struct {uint32_t magic,version,eligible,owner,serial_floor;FlRow row[30];} FlPayload;
typedef struct {unsigned abi,size,scope;int(*status)(void);} FlPreview;
typedef char Fl_wire_size[(sizeof(FlPayload)==380)?1:-1];
#endif
