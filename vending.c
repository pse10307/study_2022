/* 간단한 자판기 프로그램 (22.7.25 ~ 22.8.29)
 *
 * - 제품들을 입력 받고 구매할 제품과 돈을 넣음
 * - 잔돈만으로도 살 수 있는 제품이 있으면 다시 구매할 제품 입력
 * - 종료
 */


 /* To do...
  * - 어떤 동작을 수행 할지 입력 받아 잔돈으로 살 수 있어도 종료 할 수 있게끔
  * - 잔돈이 없어도 돈을 추가로 더 넣을 수 있게끔
  * - 각종 init 함수들은 차후 IO operation을 이용해 구현할 것
  * - C++이나 다른 Graphic 언어들을 이용..?
  * - 코드 속도 최적화 (추가적인 자료구조 사용)
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NAME_SIZE 15
#define PRODUCT_SIZE 3
#define MAX 10000

  /* Product 구조체
   * - 자판기에 들어갈 제품에 대한 구조체를 정의
   * name: 제품 이름 / stock: 제품의 재고 / price: 제품의 가격
   */
struct Product {
    char name[NAME_SIZE];
    int stock;
    int price;
};

/* Vending 구조체
 * - 자판기를 나타내는 구조체를 정의
 * name: 자판기 이름 / ProductArr: 제품의 pointer를 담는 배열 / nr_prod: 제품 종류의 수
 */
struct Vending {
    char name[NAME_SIZE];
    struct Product* ProductArr[PRODUCT_SIZE];
    int nr_prod;
};

/* User 구조체
 * - 사용자를 표기하는 구조체를 정의
 * name: 구매할 제품의 이름 / input: 사용자가 넣는 돈 / change: 구매 후 잔돈
 *
 * - input은 3가지 값을 가질 수 있음
 * - (-1): 처음 상태 == 돈을 넣어야 함 / 0: 잔돈을 이용해 제품을 구매 할 것임 / else: 넣은 돈
 */
struct User {
    char name[NAME_SIZE];
    int input;
    int change;
};

struct Product* createProduct(void);
void initProduct(struct Product*);

struct Vending* createVending(void);
void initVending(struct Vending*);

struct User* createUser(void);
void initUser(struct User*);
void GetUser(struct User*);

void PrintUser(struct User*);

int isAffordable(struct Product*, struct User*);

void PrintProduct(struct Product*);
void PrintProducts(struct Vending*);

struct Product* findProduct(struct Vending*, char[]);
void buyProduct(struct Product*, struct User*);
int updateVending(struct Vending*, struct User*);

struct Product* minProduct(struct Vending*);
struct Product* maxProduct(struct Vending*);

void PrintAffordable(struct Vending*, struct User*);

/*
 * - 각종 문자열 입력 받을 때 '\n'도 입력되는지 잘 확인할 것
 * e.g. 제품명을 입력 받을 때 "Pepsi\n\0" 형식이 아닌
 * "Pepsi\0"로 저장 되어야 함
 */

 /* createProduct()
  * 제품 구조체에 대한 memory를 할당 받음
  * arg: void
  * return: 할당 받은 제품 구조체에 대한 pointer
  */
struct Product* createProduct(void)
{
    return (struct Product*)malloc(sizeof(struct Product));
}

/* initProduct()
 * - 제품 구조체를 초기화 / 이름, 재고 수, 가격을 입력 받음
 * arg: 제품 구조체에 대한 pointer
 * return: void
 */
void initProduct(struct Product* p)
{
    char dummy[NAME_SIZE];

    printf("제품명을 입력하세요: ");
    fgets(p->name, NAME_SIZE, stdin);
    p->name[strlen(p->name) - 1] = '\0';

    printf("재고수를 입력하세요: ");
    fgets(dummy, NAME_SIZE, stdin);
    p->stock = atoi(dummy);

    dummy[0] = '\0';

    printf("가격을 입력하세요: ");
    fgets(dummy, NAME_SIZE, stdin);
    p->price = atoi(dummy);
}

/* createVending()
 * - 자판기 구조체에 대한 memory를 할당 받음
 * arg: void
 * return: 할당 받은 자판기 구조체에 대한 pointer
 */
struct Vending* createVending(void)
{
    return (struct Vending*)malloc(sizeof(struct Vending));
}

/* initVending()
 * - 자판기 구조체를 초기화
 * arg: 제품 구조체에 대한 pointer
 * return: void
 *
 * - define을 통해 정의 되어 있는 제품 수만큼 반복하며 createProduct()와
 *   initProduct()를 통해 제품을 입력 받음
 * - 입력 받은 제품을 자판기 배열에 하나씩 할당 함
 */
void initVending(struct Vending* v)
{
    int i = 0;

    struct Product* dummy;
    printf("자판기명을 입력하세요: ");
    fgets(v->name, NAME_SIZE, stdin);
    v->name[strlen(v->name) - 1] = '\0';

    for (i = 0; i < PRODUCT_SIZE; i++)
    {
        dummy = createProduct();
        initProduct(dummy);
        v->ProductArr[i] = dummy;
    }
    v->nr_prod = i;
}

/* createUser()
 * 사용자 구조체에 대한 memory를 할당 받음
 * arg: void
 * return: 할당 받은 사용자 구조체에 대한 pointer
 */
struct User* createUser(void)
{
    return (struct User*)malloc(sizeof(struct User));
}

/* initUser()
 * - 사용자 구조체를 초기화
 * arg: 사용자 구조체에 대한 pointer
 * return: void
 *
 * - 구매할 제품 명 배열을 NULL character로 초기화
 * - input, change를 &#8211;1로 초기화
 */
void initUser(struct User* u)
{
    u->name[0] = '\0';
    u->input = u->change = -1;
}

/* GetUser()
 * - 사용자에 대한 정보를 받음
 * arg: 사용자 구조체에 대한 pointer
 * return: void
 *
 * - 이 함수를 main()에서 반복 수행하며 다음 구매할 제품과 돈을 입력 받음
 *
 * - 구매할 제품 명을 입력 받음
 * - 만약 input 값이 &#8211;1이라면 처음 수행하는 것이므로 돈을 입력 받음
 * - 만약 input이 다른 값이라면 이미 정보가 들어 있다는 것이므로
 *   구매할 제품 명만 입력 받음
 * - 사용자 구조체 정의 참고
 */
void GetUser(struct User* u)
{
    char dummy[NAME_SIZE];
    printf("구매하고 싶은 제품명을 입력하세요: ");
    fgets(u->name, NAME_SIZE, stdin);
    u->name[strlen(u->name) - 1] = '\0';

    if (u->input == -1)
    {
        printf("돈을 넣어주세요: ");
        fgets(dummy, NAME_SIZE, stdin);
        u->input = atoi(dummy);
    }
}

/* isAffordable()
 * arg: 구매할 제품 구조체에 대한 pointer / 사용자 구조체에 대한 pointer
 * return: 1 == 구매 가능, 0 == 구매 불가능
 *
 * - 사용자의 input 혹은 change로 제품을 구매 할 수 있는지 확인 함
 */
int isAffordable(struct Product* p, struct User* u)
{
    if (p->price <= u->input || p->price <= u->change)
        return 1;
    else
        return 0;
}

/* PrintProduct()
 * - 제품에 대한 정보를 출력
 * arg: 출력할 제품 구조체에 대한 pointer
 * return: void
 *
 * - 제품의 각 field를 형식에 맞춰 출력
 */
void PrintProduct(struct Product* p)
{
    printf("제품명: %s\n", p->name);
    printf("재고: %d\n", p->stock);
    printf("가격: %d\n", p->price);
}

/* PrintProducts()
 * - 자판기에 있는 모든 제품들을 출력
 * arg: 출력할 자판기 구조체에 대한 pointer
 * return: void
 *
 * - 자판기에 들어 있는 제품의 종류수 (nr_pod)만큼 제품을 출력
 * - PrintProduct()를 이용해 출력
 */
void PrintProducts(struct Vending* v)
{
    int i = 0;
    printf("\n");
    for (i = 0; i < v->nr_prod; ++i)
    {
        printf("----------------------------------\n");
        printf("%d번째 제품\n", i + 1);
        PrintProduct(v->ProductArr[i]);
    }
    printf("----------------------------------\n\n");
}

/* PrintUser()
 * - 사용자에 대한 정보를 출력
 * arg: 출력할 사용자 구조체에 대한 pointer
 * return: void
 *
 * - 사용자의 각 field를 형식에 맞춰 출력
 */
void PrintUser(struct User* u)
{
    printf("\n");
    printf("구매 명: %s\n", u->name);
    printf("넣은 돈: %d\n", u->input);
    printf("잔돈: %d\n", u->change);
}

/* findProduct()
 * - 자판기에서 제품이 있는지 없는지 확인
 * arg: 자판기 구조체에 대한 pointer / 찾고 싶은 제품 이름
 * return: 같은 이름을 갖고 있는 제품 구조체에 대한 pointer / 없다면 NULL
 *
 * - 반복문과 strncmp()를 이용해 찾고 싶은 제품이 자판기에 있는지 확인
 * - 있다면 해당 제품의 pointer를 반환 / 없다면 NULL pointer를 반환
 */
struct Product* findProduct(struct Vending* v, char name[])
{
    int i;
    for (i = 0; i < v->nr_prod; i++)
        if (!strncmp(v->ProductArr[i]->name, name, strlen(name)))
            return v->ProductArr[i];
    return NULL;
}

/* buyProduct()
 * - 사용자 정보를 기반으로 제품을 구매하는 함수
 * arg: 구매할 제품 구조체에 대한 pointer / 사용자 구조체에 대한 pointer
 * return: void
 *
 * - 혹시나 argument로 NULL이 들어올 수도 있으므로 오류 check
 * - 오류가 없다면 isAffordable()을 이용해 해당 제품을 구매 가능한지 확인
 * - 구매 가능하다면 제품 명을 출력
 * - 구매할 때 사용자 input에 따라 동작이 달라짐
 *   i) input == 0
 *     - 잔돈(change)를 이용해 제품을 구매 할 것임
 *   ii) else
 *     - input을 이용해 제품을 구매
 * - 구매 후 제품의 재고를 1 감소
 */
void buyProduct(struct Product* p, struct User* u)
{
    if (!u)
    {
        printf("사용자 정보가 잘못 되었습니다\n");
        return;
    }
    if (!p)
    {
        printf("%s 라는 제품이 없습니다\n", u->name);
        return;
    }

    if (!isAffordable(p, u))
    {
        printf("돈을 더 넣어 주세요\n");
        return;
    }

    printf("%s를 구매합니다\n", u->name);

    if (u->input == 0)
    {
        u->change = u->change - p->price;
    }
    else if (u->input && u->input != -1)
    {
        u->change = u->input - p->price;
    }
    --p->stock;
}

/* updateVending()
 * - 자판기에 재고를 업데이트 / 잔돈으로 살 수 있는 제품이 있나 확인
 * arg: 자판구 구조체에 대한 pointer / 사용자 구조체 대한 pointer
 * return: 1 == 잔돈으로만 제품을 구매할 수 있음 / 0 == 잔돈을 이용해 제품을 구매할 수 없음
 *
 * - 두 가지 동작을 수행
 *   1) 자판기에 들어 있는 제품들 중 재고가 0인 제품이 있는지 확인
 *      - 만약 있다면 그 제품 뒤로 있는 제품들을 앞으로 한 칸씩 당김
 *
 *   2) 자판기에 들어 있는 제품들 중 사용자의 잔돈으로 구매할 수 있는
 *      제품이 하나라도 있는지 확인
 *      - 만약 있다면 잔돈을 이용해 구매하는 것이라 생각
 *      - 이를 표현하기 위해 input에 0을 할당   ! 사용자 구조체 정의 참고 !
 *      - 있다면 1을 반환, 없다면 0을 반환
 */
int updateVending(struct Vending* v, struct User* u)
{
    int i = 0, j = 0;
    // 1)
    for (i = 0; i < v->nr_prod; i++)
    {
        if (v->ProductArr[i]->stock == 0)
        {
            for (j = i; j < v->nr_prod - 1; j++)
                v->ProductArr[j] = v->ProductArr[j + 1];

            v->ProductArr[j] = NULL;
            --i;
            --v->nr_prod;
        }
    }
    // 2)
    for (i = 0; i < v->nr_prod; i++)
        if (v->ProductArr[i]->price <= u->change)
        {
            u->input = 0;
            return 1;
        }
    printf("\n돈이 부족합니다\n");
    return 0;
}

/* minProduct()
 * - 자판기 중 가격이 가장 싼 제품 pointer를 반환
 * arg: 자판기 구조체에 대한 pointer
 * return: 제품 구조체에 대한 pointer
 *
 * - 자판기에 들어 있는 제품들 중 가격이 가장 싼 제품을 찾고
 *   해당 제품에 대한 pointer를 반환
 */
struct Product* minProduct(struct Vending* v)
{
    int min = v->ProductArr[0]->price;
    struct Product* result = v->ProductArr[0];
	
    for (int i = 0; i<v->nr_prod; ++i)
    {
        if (v->ProductArr[i]->price < min)
        {
            min = v->ProductArr[i]->price;
            result = v->ProductArr[i];
        }
    }
    return result;
}

/* maxProduct()
 * - 자판기 중 가격이 가장 비싼 제품 pointer를 반환
 * arg: 자판기 구조체에 대한 pointer
 * return: 제품 구조체에 대한 pointer
 *
 * - 자판기에 들어 있는 제품들 중 가격이 가장 비싼 제품을 찾고
 *   해당 제품에 대한 pointer를 반환
 */
struct Product* maxProduct(struct Vending* v)
{
    int max = v->ProductArr[0]->price;
    struct Product* result = v->ProductArr[0];
	
    for (int i = 0; i<v->nr_prod; ++i)
    {
        if (v->ProductArr[i]->price > max)
        {
            max = v->ProductArr[i]->price;
            result = v->ProductArr[i];
        }
    }
    return result;
}

/* printAffordable()
 * - 자판기 중 사용자가 구매할 수 있는 제품들을 출력
 * arg: 자판기 구조체에 대한 pointer / 사용자 구조체에 대한 pointer
 * return: void
 *
 * - 반복문과 isAffordable()을 이용해 사용자가 구매할 수 있는 제품들만 출력
 */
void PrintAffordable(struct Vending* v, struct User* u)
{
    int i = 0;

    for (i; i < v->nr_prod; i++)
    {
        if (isAffordable(v->ProductArr[i], u) == 1)
            PrintProduct(v->ProductArr[i]);
    }
}

/* main()
 * - 자판기 프로그램을 수행
 * - 자판기 제품들을 입력 받음
 * - 제품들을 출력
 * - 구매할 제품, 돈을 입력 받음
 * - 제품을 구매
 * - 자판기를 update
 * - 반복
 */

int main()
{
    struct Vending* v_p = createVending();
    struct User* u_p = createUser();
    struct Product* dummy_product = NULL;
    int user_state = 1;
    initVending(v_p);
    printf("\n");
    initUser(u_p);

    /*
    while(user_state)
    {
       PrintProducts(v_p);

       GetUser(u_p);

       dummy_product = findProduct(v_p, u_p->name);
       buyProduct(dummy_product, u_p);

       PrintUser(u_p);

       user_state = updateVending(v_p, u_p);
    }*/

    PrintProducts(v_p);

    printf("\n가장 싼 제품\n");
    printf("----------------------------------\n");
    PrintProduct(minProduct(v_p));


    printf("\n가장 비싼 제품\n");
    printf("----------------------------------\n");
    PrintProduct(maxProduct(v_p));
    printf("----------------------------------\n\n");

    GetUser(u_p);

    printf("\n구매 가능한 제품\n");
    printf("------------------------------------\n\n");
    PrintAffordable(v_p, u_p);

    return 0;
}

/* by 지능 IoT 22 박세은, 이동성, 이채원, 이혜윤, 임지빈 */