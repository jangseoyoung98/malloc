#include <stdio.h>

// ※ "extern" -> 외부 참조 변수
extern int mm_init (void);
extern void *mm_malloc (size_t size);
extern void mm_free (void *ptr);
extern void *mm_realloc(void *ptr, size_t size);

/* 가용 리스트 조작을 위한 기본 상수와 매크로 정의*/

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7) // 8자리로 표현한다.

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE       4   // 워드 크기 -> header/footer
#define DSIZE       8   // 더블 워드 크기
#define CHUNKSIZE   (1<<12) // 힙 초기 사이즈 설정용 (4096바이트)
// ★ << (비트 쉬프트) 연산을 쓴 이유 : 코드의 가독성을 높이고, 유지 보수성을 개선하며, 추후에 청크 크기를 변경해야 할 때 한 번에 수정할 수 있다.

#define MAX(x, y)  ((x) > (y) ? (x) : (y))

/* Pack a and allocated bit into a word */
// PACK 매크로: 크기와 할당 비트를 통합해서(or 연산자로 합침) header와 footer에 저장할 수 있는 값 리턴
#define PACK(size, alloc)  ((size) | (alloc))

/* Read and write a word at address p */
// GET 매크로: 주소 p가 참조하는 워드를 읽는 함수 
// PUT 매크로: 주소 p가 가리키는 워드에 val을 저장하는 함수
#define GET(p)          (*(unsigned int *)(p))
#define PUT(p, val)     (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
// GET_SIZE 매크로: GET으로 읽어온 p 워드의 사이즈 정보를 읽어오는 함수
// GET_ALLOC 매크로: 사이즈 정보를 무시한 채 할당 여부를 나타내는 맨 뒷자리만 확인하는 함수
#define GET_SIZE(p)     (GET(p) & ~0x7) 
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, comput address of its header and footer */
// HDRP 매크로: bp가 속한 블록의 헤더를 알려주는 함수
// FTRP 매크로: bp가 속한 블록의 풋터를 알려주는 함수
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, comput address of next and previous blocks */
// NEXT_BLKP 매크로: 지금 블록의 bp에 블록의 크기(char*이므로 word 단위)만큼을 더한다.
// PREV_BLKP 매크로: 지금 블록의 bp에 블록의 크기(dword 단위)만큼을 뻰다.
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))   // (char *)(bp) + GET_SIZE(지금 블록의 헤더값)
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))   // (char *)(bp) - GET_SIZE(이전 블록의 풋터값)

static char *heap_listp;    // 항상 prologue block을 가리키는 정적 전역 변수 설정
static char* best_bp;


typedef struct {
    char *teamname; /* ID1+ID2 or ID1 */
    char *name1;    /* full name of first member */
    char *id1;      /* login ID of first member */
    char *name2;    /* full name of second member (if any) */
    char *id2;      /* login ID of second member */
} team_t;

extern team_t team;
