; Listing generated by Microsoft (R) Optimizing Compiler Version 19.00.24215.1 

	TITLE	E:\Egor\projects\GameEngine\lifeEngine\lifeEngine\Utils\OpenGL\glm\glm\detail\dummy.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB MSVCRTD

PUBLIC	_main
EXTRN	__RTC_InitBase:PROC
EXTRN	__RTC_Shutdown:PROC
;	COMDAT rtc$TMZ
rtc$TMZ	SEGMENT
__RTC_Shutdown.rtc$TMZ DD FLAT:__RTC_Shutdown
rtc$TMZ	ENDS
;	COMDAT rtc$IMZ
rtc$IMZ	SEGMENT
__RTC_InitBase.rtc$IMZ DD FLAT:__RTC_InitBase
rtc$IMZ	ENDS
; Function compile flags: /Odtp /RTCsu
; File e:\egor\projects\gameengine\lifeengine\lifeengine\utils\opengl\glm\glm\detail\dummy.cpp
_TEXT	SEGMENT
_main	PROC

; 190  : {

	push	ebp
	mov	ebp, esp

; 191  : /*
; 192  : 	glm::vec1 o(1);
; 193  : 	glm::vec2 a(1);
; 194  : 	glm::vec3 b(1);
; 195  : 	glm::vec4 c(1);
; 196  : 
; 197  : 	glm::quat q;
; 198  : 	glm::dualquat p;
; 199  : 
; 200  : 	glm::mat4 m(1);
; 201  : 
; 202  : 	float a0 = normalizeDotA(a, a);
; 203  : 	float b0 = normalizeDotB(b, b);
; 204  : 	float c0 = normalizeDotC(c, c);
; 205  : */
; 206  : 	return 0;

	xor	eax, eax

; 207  : }

	pop	ebp
	ret	0
_main	ENDP
_TEXT	ENDS
END
