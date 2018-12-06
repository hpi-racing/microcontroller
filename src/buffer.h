#ifndef BUFFER_H_
#define BUFFER_H_

#define DECLARE_BUFFER(queueName,itemType,queueLength) \
	itemType		g_##queueName[queueLength]; \
	itemType*		g_p##queueName##EndOfQueue = g_##queueName + queueLength - 1; \
	itemType*		g_p##queueName##Write = g_##queueName; \
	itemType*		g_p##queueName##Read = g_##queueName; \
	static const unsigned char	g_uc##queueName##Length = queueLength; \
	unsigned char	g_uc##queueName##NumberOfFreeEntries = queueLength; \
	
	
#define BUFFER_FREEENTRIES(queueName) \
	(g_uc##queueName##NumberOfFreeEntries)

#define BUFFER_CANWRITE(queueName) \
	(g_uc##queueName##NumberOfFreeEntries > 0)

#define BUFFER_WRITE(queueName,data) \
{ \
	*g_p##queueName##Write = data; \
	if (g_p##queueName##Write == g_p##queueName##EndOfQueue) \
		g_p##queueName##Write = g_##queueName; \
	else \
		g_p##queueName##Write++; \
	g_uc##queueName##NumberOfFreeEntries--; \
} 
	
#define BUFFER_CANREAD(queueName) \
	(g_uc##queueName##NumberOfFreeEntries != g_uc##queueName##Length)
	
#define BUFFER_READ(queueName) \
	*g_p##queueName##Read

#define BUFFER_MOVENEXT(queueName) \
{ \
	if (g_p##queueName##Read == g_p##queueName##EndOfQueue) \
		g_p##queueName##Read = g_##queueName; \
	else \
		g_p##queueName##Read++; \
	g_uc##queueName##NumberOfFreeEntries++; \
}	

#endif 