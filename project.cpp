#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "orders.txt"

typedef struct Order {
    char orderID[20];
    char customerName[50];
    char product[50];
    int quantity;
    float totalPrice;
    char status[20];
} Order;

typedef struct Node {
    Order *order;
    struct Node *next;
} Node;

typedef struct {
    Node *front, *rear;
} Queue;

void initializeQueue(Queue *q) {
    q->front = q->rear = NULL;
}

int isEmpty(Queue *q) {
    return q->front == NULL;
}

void enqueue(Queue *q, Order order)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->order = (Order *)malloc(sizeof(Order));
    if (!newNode || !newNode->order)
    {
        printf("Khong the cap phat bo nho!\n");
        return;
    }
    *(newNode->order) = order;
    newNode->next = NULL;

    if (isEmpty(q))
    {
        q->front = q->rear = newNode;
    }
    else
    {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

Order dequeue(Queue *q) {
    Order emptyOrder = {"", "", "", 0, 0.0, ""};
    if (isEmpty(q)) {
        printf("Danh sach rong!\n");
        return emptyOrder;
    }
    Node *temp = q->front;
    Order removedOrder = *(temp->order);
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    free(temp->order);
    free(temp);
    return removedOrder;
}

void processOrder(Queue *q)
{
    if (isEmpty(q))
    {
        printf("Danh sach rong!\n");
        return;
    }

    Node *temp = q->front;

    // Tìm đơn hàng đầu tiên chưa được xử lý
    while (temp && strcmp(temp->order->status, "Da xu ly") == 0)
    {
        temp = temp->next;
    }

    if (!temp)
    {
        printf("Tat ca don hang da xu ly!\n");
        return;
    }

    // Xử lý đơn hàng chưa xử lý
    strcpy(temp->order->status, "Da xu ly");
    printf("Don hang %s da duoc xu ly!\n", temp->order->orderID);
}

void displayQueue(Queue *q) {
    if (isEmpty(q)) {
        printf("Danh sach rong!\n");
        return;
    }
    printf("Danh sach don hang:\n");
    Node *temp = q->front;
    while (temp) {
        printf("Ma don: %s | Khach: %s | San pham: %s | So luong: %d | Tong gia: %.2f | Trang thai: %s\n",
            temp->order->orderID, temp->order->customerName, temp->order->product, temp->order->quantity, temp->order->totalPrice, temp->order->status);
        temp = temp->next;
    }
}

void cancelOrder(Queue *q, char *orderID)
{
    if (isEmpty(q))
    {
        printf("Danh sach rong!\n");
        return;
    }
    Node *temp = q->front, *prev = NULL;
    while (temp && strcmp(temp->order->orderID, orderID) != 0)
    {
        prev = temp;
        temp = temp->next;
    }
    if (!temp)
    {
        printf("Khong tim thay ma don %s!\n", orderID);
        return;
    }
    if (strcmp(temp->order->status, "Da xu ly") == 0)
    {
        printf("Don hang %s da xu ly, khong the huy!\n", orderID);
        return;
    }
    if (!prev)
        q->front = temp->next;
    else
        prev->next = temp->next;
    if (temp == q->rear)
        q->rear = prev;
    free(temp->order);
    free(temp);
    printf("Da huy don hang %s!\n", orderID);
}

void saveToFile(Queue *q)
{
    FILE *file = fopen(FILENAME, "w");
    if (!file)
    {
        printf("Loi mo file!\n");
        return;
    }
    Node *temp = q->front;
    while (temp)
    {
        fprintf(file, "%s %s %s %d %.2f %s\n",
                temp->order->orderID, temp->order->customerName, temp->order->product,
                temp->order->quantity, temp->order->totalPrice, temp->order->status);
        temp = temp->next;
    }
    fclose(file);
}

void loadFromFile(Queue *q)
{
    FILE *file = fopen(FILENAME, "r");
    if (!file)
        return;

    while (!feof(file))
    {
        Order *order = (Order *)malloc(sizeof(Order));
        if (fscanf(file, "%s %s %s %d %f %[^\n]", 
                order->orderID, order->customerName, order->product, 
                &order->quantity, &order->totalPrice, order->status) == 6)
        {
            enqueue(q, *order);
        }
        else
        {
            free(order);
        }
    }
    fclose(file);
}

int main()
{
    Queue q;
    initializeQueue(&q);
    loadFromFile(&q);
    int choice;
    Order order;
    char orderID[20];
    do
    {
        printf("\nMENU:\n");
        printf("1. Them don hang\n");
        printf("2. Hien thi danh sach don hang\n");
        printf("3. Xu ly don hang\n");
        printf("4. Huy don hang\n");
        printf("5. Thoat\n");
        printf("Chon: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            printf("Nhap ma don: ");
            scanf("%s", order.orderID);
            getchar();  // Xóa ký tự '\n' còn lại trong bộ đệm

            printf("Nhap ten khach: ");
            fgets(order.customerName, sizeof(order.customerName), stdin);
            order.customerName[strcspn(order.customerName, "\n")] = '\0';

            printf("Nhap san pham: ");
            fgets(order.product, sizeof(order.product), stdin);
            order.product[strcspn(order.product, "\n")] = '\0'; 

            printf("Nhap so luong: ");
            scanf("%d", &order.quantity);

            printf("Nhap tong gia: ");
            scanf("%f", &order.totalPrice);
            getchar();

            strcpy(order.status, "Dang xu ly");

            enqueue(&q, order);
            saveToFile(&q);
            break;
        case 2:
            displayQueue(&q);
            break;
        case 3:
            processOrder(&q);
            saveToFile(&q);
            break;
        case 4:
            printf("Nhap ma don can huy: ");
            scanf("%s", orderID);
            cancelOrder(&q, orderID);
            saveToFile(&q);
            break;
        case 5:
            printf("Thoat chuong trinh!\n");
            break;
        default:
            printf("Lua chon khong hop le!\n");
        }
    } while (choice != 5);

    return 0;
}