#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <conio.h>
#include <limits.h>
#define maxVertex 10

typedef struct nodeLL {
    char namaJln[20], kondisiJln[30];
    int durasi, panjang, bobot;
    struct nodeLL *lokasi_sebelum, *lokasi_setelah;
} node;

node *head = NULL;
node *tail = NULL;

//Deklarasi GRAPH dan ARRAY nama_jalan
int matriks_tetangga[maxVertex][maxVertex];
char GraphnamaJalan[maxVertex][maxVertex][25];

//Deklarasi Array Bantu
int dist[maxVertex][maxVertex];
int jalur[maxVertex][maxVertex];

void IntoLinkedList(node *LL);
void lihatKondisiJalur();
void editKondisiJalur();
void hlmAwal();
void adminMenu();
void userMenu();
void initializeGraphBobot();
void initializeNamaJalan();
void bobotToGraph();
void printSolution(int dist[][maxVertex], int jalur[][maxVertex], int awal, int akhir);
void cariRuteTerbaik();
char* cekKota(int indeks);

void IntoLinkedList(node* LL){
    // Inisialisasi FILE dan declare VAR
    FILE *maps;
    char data[150];
    char jalan[20], kondisii[30];
    int durasii, panjangg, bobott;

    //Buka FILE dgn mode "READ"
    maps = fopen("GPS.txt", "r");
    if (maps == NULL){
        fclose(maps);
        printf("\n*\n");
        return;
    }

    //Baca FILE txt
    while(fgets(data, 150, maps) != NULL){
        char *token = strtok(data, ";");
        strcpy(jalan, token);

        token = strtok(NULL, ";");
        strcpy(kondisii, token);

        token = strtok(NULL, ";");
        durasii = atoi(token);

        token = strtok(NULL, ";");
        panjangg = atoi(token);

        token = strtok(NULL, ";");
        bobott = atoi(token);

        node *nodeBaru = (node*) malloc(sizeof(node));
        strcpy(nodeBaru->namaJln, jalan);
        strcpy(nodeBaru->kondisiJln, kondisii);
        nodeBaru->durasi = durasii;
        nodeBaru->panjang = panjangg;
        nodeBaru->bobot = bobott;

        if (head == NULL){
            nodeBaru->lokasi_sebelum = NULL;
            nodeBaru->lokasi_setelah = NULL;
            head = tail = nodeBaru;
        }
        else {
            tail->lokasi_setelah = nodeBaru;
            nodeBaru->lokasi_sebelum = tail;
            nodeBaru->lokasi_setelah = NULL;
            tail = nodeBaru;
        }
    }
    //tutup FILE
    fclose(maps);
}

void lihatKondisiJalur(){
    if (head == NULL){
        printf("\n*\n");
        return;
    }

    int i = 1;
    node *current = (node*) malloc(sizeof(node));
    current = head;
    printf("\nNO\tJALAN/KOTA\t\tKONDISI LALU LINTAS\t\tDURASI NORMAL\tPANJANG JALAN\tWAKTU TEMPUH\n");
    printf("==========================================================================================================\n");
    while (current != NULL){
        if (strlen(current->namaJln) > 7){
            if (strlen(current->kondisiJln) > 7 ){
                printf("%d\t%s\t\t%s\t\t\t%d\t\t%d\t\t%d\n", i, current->namaJln, current->kondisiJln, current->durasi, current->panjang, current->bobot);
                current = current->lokasi_setelah;
                i += 1;
            }
            else {
                printf("%d\t%s\t\t%s\t\t\t\t%d\t\t%d\t\t%d\n", i, current->namaJln, current->kondisiJln, current->durasi, current->panjang, current->bobot);
                current = current->lokasi_setelah;
                i += 1;
            }
        }
        else{
            printf("%d\t%s\t\t\t%s\t\t\t\t%d\t\t%d\t\t%d\n", i, current->namaJln, current->kondisiJln, current->durasi, current->panjang, current->bobot);
            current = current->lokasi_setelah;
            i += 1;
        }
    }
    free(current);
    printf("\n");
}

void editKondisiJalur(){
    if (head == NULL){
        printf("\n*\n");
        getch();
        return;
    }

    char nmJalan[20], kondisii[30];
    int durasii;
    fflush(stdin);
    printf("Masukkan nama jalan yang datanya ingin diubah: ");
    gets(nmJalan); fflush(stdin);

    //baca linked list
    node *temp = (node*)malloc(sizeof(node));
    temp = head;
    while (temp != NULL){
        if (strcasecmp(nmJalan, temp->namaJln) == 0){
            printf("\n====== Data Jalur Terbaru ======");
            printf("\nJalan\t\t\t: %s\n", temp->namaJln);

            printf("Panjang jalan\t\t: %d\n", temp->panjang);

            printf("Durasi perjalanan normal: %d\n", temp->durasi);

            printf("Kondisi lalu lintas\t: ");
            gets(kondisii); fflush(stdin);
            strcpy(temp->kondisiJln, kondisii);

            int bobotbaru;
            if (strcasecmp(kondisii, "macet") == 0){
                bobotbaru = (2*temp->panjang) + temp->durasi;
            }
            else if (strcasecmp(kondisii, "agak lancar") == 0){
                bobotbaru = temp->panjang + temp->durasi;
            }
            else if (strcasecmp(kondisii, "lancar") == 0){
                bobotbaru = temp->durasi;
            }

            printf("Waktu Tempuh Baru\t: %d\n", bobotbaru);
            temp->bobot = bobotbaru;

            getch();
            break;
        }
        temp = temp->lokasi_setelah;
    }

    //buka FILE
    FILE *maps;
    maps = fopen("GPS.txt", "w");
    temp = head;
    while (temp != NULL){
        fprintf(maps, "%s;%s;%d;%d;%d\n", temp->namaJln, temp->kondisiJln
                , temp->durasi, temp->panjang, temp->bobot);
        temp = temp->lokasi_setelah;
    }

    //tutup FILE
    fclose(maps);
}

void initializeGraphBobot(){
    int i, j;

    for (i = 0; i < maxVertex; i++){
        for (j = 0; j < maxVertex; j++){
            matriks_tetangga[i][j] = INT_MAX;
        }
    }
}

void initializeNamaJalan(){
    int i, j;

    for (i = 0; i < maxVertex; i++){
        for (j = 0; j < maxVertex; j++){
            strcpy(GraphnamaJalan[i][j], "");
        }
    }
}

void bobotToGraph(){
    if (head == NULL){
        printf("\n*\n");
        return;
    }

    /*  JAKARTA UTARA       = 0
        JAKARTA BARAT       = 1
        JAKARTA PUSAT       = 2
        JAKARTA TIMUR       = 3
        JAKARTA SELATAN     = 4
        TANGERANG           = 5
        TANGERANG SELATAN   = 6
        BEKASI              = 7
        DEPOK               = 8
        BOGOR               = 9             */

    //Masukkin hubungan linked list ke graph
    node *temp = (node*)malloc(sizeof(node));
    temp = head;
    while (temp != NULL){
        if (strcasecmp(temp->namaJln, "Raya Dadap") == 0){
            matriks_tetangga[0][5] = temp->bobot;
            matriks_tetangga[5][0] = temp->bobot;
            strcpy(GraphnamaJalan[0][5], temp->namaJln);
            strcpy(GraphnamaJalan[5][0], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "P. Jayakarta") == 0){
            matriks_tetangga[0][1] = temp->bobot;
            matriks_tetangga[1][0] = temp->bobot;
            strcpy(GraphnamaJalan[0][1], temp->namaJln);
            strcpy(GraphnamaJalan[1][0], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Gn. Sahari") == 0){
            matriks_tetangga[0][2] = temp->bobot;
            matriks_tetangga[2][0] = temp->bobot;
            strcpy(GraphnamaJalan[0][2], temp->namaJln);
            strcpy(GraphnamaJalan[2][0], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Kelapa Gading") == 0){
            matriks_tetangga[0][3] = temp->bobot;
            matriks_tetangga[3][0] = temp->bobot;
            strcpy(GraphnamaJalan[0][3], temp->namaJln);
            strcpy(GraphnamaJalan[3][0], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Bojong") == 0){
            matriks_tetangga[0][7] = temp->bobot;
            matriks_tetangga[7][0] = temp->bobot;
            strcpy(GraphnamaJalan[0][7], temp->namaJln);
            strcpy(GraphnamaJalan[7][0], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Daan Mogot") == 0){
            matriks_tetangga[1][5] = temp->bobot;
            matriks_tetangga[5][1] = temp->bobot;
            strcpy(GraphnamaJalan[1][5], temp->namaJln);
            strcpy(GraphnamaJalan[5][1], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Raya Serpong") == 0){
            matriks_tetangga[5][6] = temp->bobot;
            matriks_tetangga[6][5] = temp->bobot;
            strcpy(GraphnamaJalan[5][6], temp->namaJln);
            strcpy(GraphnamaJalan[6][5], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Slipi") == 0){
            matriks_tetangga[1][2] = temp->bobot;
            matriks_tetangga[2][1] = temp->bobot;
            strcpy(GraphnamaJalan[1][2], temp->namaJln);
            strcpy(GraphnamaJalan[2][1], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Pal Merah") == 0){
            matriks_tetangga[1][4] = temp->bobot;
            matriks_tetangga[4][1] = temp->bobot;
            strcpy(GraphnamaJalan[1][4], temp->namaJln);
            strcpy(GraphnamaJalan[4][1], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Kwitang") == 0){
            matriks_tetangga[2][3] = temp->bobot;
            matriks_tetangga[3][2] = temp->bobot;
            strcpy(GraphnamaJalan[2][3], temp->namaJln);
            strcpy(GraphnamaJalan[3][2], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Sudirman") == 0){
            matriks_tetangga[2][4] = temp->bobot;
            matriks_tetangga[4][2] = temp->bobot;
            strcpy(GraphnamaJalan[2][4], temp->namaJln);
            strcpy(GraphnamaJalan[4][2], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Setu Cipayung") == 0){
            matriks_tetangga[3][7] = temp->bobot;
            matriks_tetangga[7][3] = temp->bobot;
            strcpy(GraphnamaJalan[3][7], temp->namaJln);
            strcpy(GraphnamaJalan[7][3], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "TB Simatupang") == 0){
            matriks_tetangga[3][4] = temp->bobot;
            matriks_tetangga[4][3] = temp->bobot;
            strcpy(GraphnamaJalan[3][4], temp->namaJln);
            strcpy(GraphnamaJalan[4][3], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Jambore") == 0){
            matriks_tetangga[3][8] = temp->bobot;
            matriks_tetangga[8][3] = temp->bobot;
            strcpy(GraphnamaJalan[3][8], temp->namaJln);
            strcpy(GraphnamaJalan[8][3], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Ciputat") == 0){
            matriks_tetangga[4][6] = temp->bobot;
            matriks_tetangga[6][4] = temp->bobot;
            strcpy(GraphnamaJalan[4][6], temp->namaJln);
            strcpy(GraphnamaJalan[6][4], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Andara") == 0){
            matriks_tetangga[4][8] = temp->bobot;
            matriks_tetangga[8][4] = temp->bobot;
            strcpy(GraphnamaJalan[4][8], temp->namaJln);
            strcpy(GraphnamaJalan[8][4], temp->namaJln);
        }

        if (strcasecmp(temp->namaJln, "Parung") == 0){
            matriks_tetangga[8][9] = temp->bobot;
            matriks_tetangga[9][8] = temp->bobot;
            strcpy(GraphnamaJalan[8][9], temp->namaJln);
            strcpy(GraphnamaJalan[9][8], temp->namaJln);
        }

        temp = temp->lokasi_setelah;
    }

}

void floydWarshall(int graph[][maxVertex]){
    // INISIALISASI MATRIKS JARAK DAN
    for (int i=0; i<maxVertex; i++){
        for (int j=0; j<maxVertex; j++){
            dist[i][j] = graph[i][j];
            if (i == j || graph[i][j] == INT_MAX){
                jalur[i][j] = -1;
            }
            else {
                jalur[i][j] = i;
            }
        }
    }

    // Mencari jalur terpendek
    for (int k=0; k<maxVertex; k++){
        for (int i=0; i<maxVertex; i++){
            for (int j=0; j<maxVertex; j++){
                if (dist[i][k] != INT_MAX &&
                    dist[k][j] != INT_MAX &&
                    dist[i][k] + dist[k][j] < dist[i][j]){
                        dist[i][j] = dist[i][k] + dist[k][j];
                        jalur[i][j] = jalur[k][j];
                    }
            }
        }
    }

}

void printJalur(int jalur[][maxVertex], int a, int b, int *totalWaktu) {
    if (jalur[a][b] == a) {
        *totalWaktu = *totalWaktu + matriks_tetangga[a][b];
        printf("\n%s ", GraphnamaJalan[a][b]);
    } else {
        int tengahJalur = jalur[a][b];
        printJalur(jalur, a, tengahJalur, totalWaktu);
        *totalWaktu = *totalWaktu + matriks_tetangga[tengahJalur][b];
        printf("--> %s ", GraphnamaJalan[tengahJalur][b]);
    }
}

char* cekKota(int indeks){
    if (indeks == 0){
        return "Jakarta Utara";
    }
    else if (indeks == 1){
        return "Jakarta Barat";
    }
    else if (indeks == 2){
        return "Jakarta Pusat";
    }
    else if (indeks == 3){
        return "Jakarta Timur";
    }
    else if (indeks == 4){
        return "Jakarta Selatan";
    }
    else if (indeks == 5){
        return "Tangerang";
    }
    else if (indeks == 6){
        return "Tangerang Selatan";
    }
    else if (indeks == 7){
        return "Bekasi";
    }
    else if (indeks == 8){
        return "Depok";
    }
    else if (indeks == 9){
        return "Bogor";
    }

    return "No Such City";
}

void printSolution(int dist[][maxVertex], int jalur[][maxVertex], int awal, int akhir){
    int i = awal - 1;
    int j = akhir - 1;

    char kotaAwal[25], kotaAkhir[25];
    int totalWaktuTempuh = 0;
    strcpy(kotaAwal, cekKota(i));
    strcpy(kotaAkhir, cekKota(j));

    if (i != j && dist[i][j] != INT_MAX) {
        printf("\n================s==============================================");
        printf("\nRute terbaik dari [%s] menuju [%s]: ", kotaAwal, kotaAkhir);
        printf("\n===============================================================\n");
        printJalur(jalur, i, j, &totalWaktuTempuh);
        printf("\n");
    }
    printf("\nTotal waktu tempuh: %d menit [%.2f jam]", totalWaktuTempuh, ((float) totalWaktuTempuh/60) );
}

void cariRuteTerbaik(){
    bobotToGraph();
    floydWarshall(matriks_tetangga);

    system("cls");
    int kotaAwal, kotaAkhir;
    printf("1. Jakarta Utara\n");
    printf("2. Jakarta Barat\n");
    printf("3. Jakarta Pusat\n");
    printf("4. Jakarta Timur\n");
    printf("5. Jakarta Selatan\n");
    printf("6. Tangerang\n");
    printf("7. Tangerang Selatan\n");
    printf("8. Bekasi\n");
    printf("9. Depok\n");
    printf("10. Bogor\n");

    printf("\nPilih lokasi awal anda\t\t: ");
    scanf("%d", &kotaAwal);
    if (kotaAwal < 1 || kotaAwal > 10){
        printf("\n");
        system("cls");
        cariRuteTerbaik();
    }

    printf("Pilih lokasi tujuan anda\t: ");
    scanf("%d", &kotaAkhir);
    if (kotaAkhir < 1 || kotaAkhir > 10){
        printf("\n");
        system("cls");
        cariRuteTerbaik();
    }

    printSolution(dist, jalur, kotaAwal, kotaAkhir);
    return;
}

void hlmAwal(){
    int option;
    do
    {
        system("cls");
        printf("=========================\n");
        printf("===SISTEM NAVIGASI GPS===\n");
        printf("=========================\n");
        printf("\n[1] Masuk Sebagai Admin");
        printf("\n[2] Masuk Sebagai Pengguna");
        printf("\n[3] Exit");
        printf("\nMasukan Pilihan (1-3): ");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            adminMenu();
            break;
        case 2:
            userMenu();
            break;
        case 3:
            exit(1);
        default:
            printf("\nPilihan Tidak Tersedia!");
            sleep(2);
            break;
        }
    } while (option != 3);
}

void adminMenu()
{
    int option;
    do
    {
        system("cls");
        printf("=========================\n");
        printf("===SISTEM NAVIGASI GPS===\n");
        printf("=========================\n");
        printf("\n[1] Lihat Kondisi Jalur");
        printf("\n[2] Edit Kondisi Jalur");
        printf("\n[3] Kembali ke Menu Utama");
        printf("\nMasukan Pilihan (1-3): ");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            system("cls");
            lihatKondisiJalur();
            getch();
            break;
        case 2:
            system("cls");
            editKondisiJalur();
            break;
        case 3:
            hlmAwal();
            break;
        default:
            printf("\nPilihan Tidak Tersedia!");
            sleep(2);
            break;
        }
    } while (option != 3);
}

void userMenu()
{
    int option;
    do
    {
        system("cls");
        printf("=========================\n");
        printf("===SISTEM NAVIGASI GPS===\n");
        printf("=========================\n");
        printf("\n[1] Lihat Kondisi jalur");
        printf("\n[2] Cari Rute Terbaik");
        printf("\n[3] Kembali ke Menu Utama");
        printf("\nMasukan Pilihan (1-3): ");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            system("cls");
            lihatKondisiJalur();
            getch();
            break;
        case 2:
            //system("cls");
            cariRuteTerbaik();
            getch();
            break;
        case 3:
            hlmAwal();
            break;
        default:
            printf("\nPilihan Tidak Tersedia!");
            sleep(2);
            break;
        }
    } while (option != 3);
}

int main()
{
    node linkedList;
    IntoLinkedList(&linkedList);
    initializeGraphBobot();
    initializeNamaJalan();

    hlmAwal();

    return 0;
}
