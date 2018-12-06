#ifndef READ_ACCESS_H_
#define READ_ACCESS_H_

typedef enum read_access_enum
{
	READ_ACCESS_NOT_READY = 0,
	READ_ACCESS_READY = 1,
	READ_ACCESS_IN_PROGRESS = 2
} read_access_t;

#endif /* READ_ACCESS_H_ */