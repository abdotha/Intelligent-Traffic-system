#ifndef __BIT_MASKING_H__
#define __BIT_MASKING_H__

#define SET_BIT(REG,BIT)		(REG |= (1 << BIT))
#define CLEAR_BIT(REG,BIT)		(REG &= ~(1 << BIT))
#define TOG_BIT(REG,BIT)		(REG ^= (1 << BIT))
#define GET_BIT(REG,BIT)		((REG >> BIT) & 0x01)

#endif
