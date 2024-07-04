.section .SE_Key_Data,"a",%progbits
.syntax unified
.thumb
	.global SE_ReadKey_1
SE_ReadKey_1:
	PUSH {R1-R5}
	MOVW R1, #0x3145
	MOVT R1, #0x7138
	MOVW R2, #0x6c58
	MOVT R2, #0x5434
	MOVW R3, #0x6852
	MOVT R3, #0x3142
	MOVW R4, #0x6e63
	MOVT R4, #0x4c73
	STM R0, {R1-R4}
	POP {R1-R5}
	BX LR

	.global SE_ReadKey_1_Pub
SE_ReadKey_1_Pub:
	PUSH {R1-R5}
	MOVW R1, #0x7a72
	MOVT R1, #0x7cc7
	MOVW R2, #0x296f
	MOVT R2, #0x2d3d
	MOVW R3, #0xd9ce
	MOVT R3, #0x9f1b
	MOVW R4, #0x4f9a
	MOVT R4, #0xdd16
	STM R0, {R1-R4}
	ADD R0, R0,#16
	MOVW R1, #0x893f
	MOVT R1, #0xcc70
	MOVW R2, #0x9a14
	MOVT R2, #0x5009
	MOVW R3, #0xaed0
	MOVT R3, #0x7adb
	MOVW R4, #0x1f47
	MOVT R4, #0xea5d
	STM R0, {R1-R4}
	ADD R0, R0,#16
	MOVW R1, #0x66f5
	MOVT R1, #0x32b1
	MOVW R2, #0xe0f9
	MOVT R2, #0xcbab
	MOVW R3, #0xc205
	MOVT R3, #0x458
	MOVW R4, #0xfb0e
	MOVT R4, #0x2bcc
	STM R0, {R1-R4}
	ADD R0, R0,#16
	MOVW R1, #0x2be5
	MOVT R1, #0x345
	MOVW R2, #0x2034
	MOVT R2, #0xf78d
	MOVW R3, #0xb433
	MOVT R3, #0xc846
	MOVW R4, #0xdd96
	MOVT R4, #0xbf9
	STM R0, {R1-R4}
	POP {R1-R5}
	BX LR

.end
