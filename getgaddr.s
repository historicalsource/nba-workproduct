;; 
;;		$Archive: /video/Nba/getgaddr.s $
;;		$Revision: 2 $
;;		$Date: 4/20/98 11:54p $
;;
;;		Copyright (c) 1997, 1998 Midway Games Inc.
;;		All Rights Reserved
;;
;;		This file is confidential and a trade secret of Midway Games Inc.
;;		Use, reproduction, adaptation, distribution, performance or
;;		display of this computer program or the associated audiovisual work
;; 		is strictly forbidden unless approved in writing by Midway Games Inc.
;; 
	
bss	group	bss

	section	.text

	xdef	ss_getgaddr_s
	xdef	get_text_addr
	xdef	get_text_end_addr
	xref	start
	xref	__tend
	
ss_getgaddr_s
	db	"$Workfile: getgaddr.s $ $Revision: 2 $", 0

;;
;;		GLOBAL FUNCTIONS
;;

get_text_addr:
	la	r2,start
	jr	r31
	nop

get_text_end_addr:
	la	r2,__tend
	jr	r31
	nop
