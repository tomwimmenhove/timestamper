#!/bin/bash

ADDRESS="042"

sed -e "s/^Byte//g;s/
	REG=`echo $A |  awk '{ print $2 }'`
	BITS=`echo $A |  awk '{ print $4 }'`
	DEC="$((2#$BITS))"
	HEX=`printf '0x%02x\n' $DEC`

	#echo "write_reg($ADDRESS, $REG, $HEX);"
	echo -ne "$HEX, "
done)