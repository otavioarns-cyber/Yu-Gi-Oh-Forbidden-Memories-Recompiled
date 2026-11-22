#ifndef HAL20_QS_SCHEMA
#define HAL20_QS_SCHEMA
#include <stdint.h>
typedef struct {uint16_t deck,block;uint32_t female;} QsRow;
typedef struct {uint32_t magic,version,eligible,mask;uint8_t fulfilled[2],owner,pad;QsRow row[30];} QsPayload;
typedef struct {uint32_t abi,size,scope;int(*status)(void);} QsPreview;
typedef char Qs_no_pointer_wire_size[(sizeof(QsPayload)==260)?1:-1];
#endif
