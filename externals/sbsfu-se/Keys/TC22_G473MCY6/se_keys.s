.section .SE_Key_Data,"a",%progbits
.syntax unified
.thumb
	.global SE_ReadKey_1
SE_ReadKey_1:
	PUSH {R1-R5}
	MOVW R1, #0x454b
	MOVT R1, #0x5f59
	MOVW R2, #0x4541
	MOVT R2, #0x5f53
	MOVW R3, #0x4354
	MOVT R3, #0x325f
	MOVW R4, #0x5f32
	MOVT R4, #0x4244
	STM R0, {R1-R4}
	POP {R1-R5}
	BX LR

	.global SE_ReadKey_1_Pub
SE_ReadKey_1_Pub:
	PUSH {R1-R5}
	MOVW R1, #0x7b78
	MOVT R1, #0xb3d8
	MOVW R2, #0xd6f0
	MOVT R2, #0xca6f
	MOVW R3, #0x9640
	MOVT R3, #0x1e2a
	MOVW R4, #0x6bea
	MOVT R4, #0xc710
	STM R0, {R1-R4}
	ADD R0, R0,#16
	MOVW R1, #0xbc41
	MOVT R1, #0x1c4e
	MOVW R2, #0x821e
	MOVT R2, #0x9d5a
	MOVW R3, #0xb7f5
	MOVT R3, #0x9e6b
	MOVW R4, #0x8113
	MOVT R4, #0xd34d
	STM R0, {R1-R4}
	ADD R0, R0,#16
	MOVW R1, #0xfe60
	MOVT R1, #0xd68f
	MOVW R2, #0x461c
	MOVT R2, #0xc7bc
	MOVW R3, #0x2f4f
	MOVT R3, #0x43bb
	MOVW R4, #0x8dfc
	MOVT R4, #0x775b
	STM R0, {R1-R4}
	ADD R0, R0,#16
	MOVW R1, #0xa75
	MOVT R1, #0x3452
	MOVW R2, #0x1c97
	MOVT R2, #0x6ef3
	MOVW R3, #0xa242
	MOVT R3, #0xa4ff
	MOVW R4, #0x8b48
	MOVT R4, #0x1857
	STM R0, {R1-R4}
	POP {R1-R5}
	BX LR

.end
