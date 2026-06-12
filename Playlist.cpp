/*

    1) Criar um Gerenciador de músicas de uma playlist, mas com comportamento de
    edição livre, como um editor de playlist de DJ ou de streaming. Você criará um
    arquivo TXT contendo um ID único, título, Artista, Ano, Álbum, Tipo de Versão
    e ID_Original. Você poderá inserir outras músicas com os mesmos dados,
    sempre colocando na última posição do arquivo. No entanto, ao ser lido e
    colocado em uma lista, poderá inserir, remover, mover, buscar, embaralhar ou
    ordenar. Inclusive gravar outro arquivo com algum nome que desejar, por
    exemplo: Lista Por Ano ou Lista Por Álbum ou Lista Aleatória etc. Quando for
    abrir, poderá escolher qual arquivo TXT deseja. No entanto, o TXT original, com
    todas as músicas deve se manter intacto, na mesma sequência de criação.
    Portanto, o programa deve permitir inserir ou remover músicas no arquivo
    original. A partir dele, ao escolher criar uma lista Por Ano, deverá permitir gravar
    um outro arquivo contendo essas informações organizadas por ano. Isso
    também deve ser adotado para as outras formas de organização das listas
    dinâmicas.
    A ideia central é que o usuário pode:
    Inserir uma música em qualquer posição da lista, não só no início ou no fim,
    simulando quando você quer que uma faixa toque em um momento específico
    da playlist.
    Remover qualquer música pelo nome, posição ou artista, exigindo percurso e
    manipulação de ponteiros em pontos arbitrários.
    Mover uma música de uma posição para outra, o que é a operação mais rica,
    pois envolve remover o nó de um lugar e reinseri-lo em outro sem perder a
    cadeia.
    Buscar por artista e listar todas as faixas dele que aparecem na playlist,
    praticando percurso completo.
    Embaralhar a ordem de forma simulada, trocando posições de nós, o que é
    um exercício avançado de manipulação de ponteiros.
    Ordenar por Ordem Alfabética da Música: o intuito é montar uma playlist
    independentemente do estilo/gênero ou armazenamento.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define TMBUF 256
#define TMNOME 128

char nomArq[TMNOME] = "Nenhum";
char nomPlaylist[TMNOME] = "Nenhum";

void lbt() {

    char c;

    while((c = getchar()) != '\n' && c != EOF);

}

void definirArq() {

    printf("Digite o nome do arquivo (ex: dados.txt): ");

    scanf(" %127[^\n]", nomArq); // Aceita qualquer char exceto '\n', max 127

    if (strlen(nomArq) == 0) {

        printf("Nome inválido! Usando 'arquivo.txt' como padrão.\n");
        strcpy(nomArq, "arquivo.txt");

    }

    printf("Arquivo definido: '%s'\n\n", nomArq);

}

void gravarLinha() {

    if (strlen(nomArq) == 0) {

        printf("Nenhum arquivo definido! Escolha a opção 1 primeiro.\n");

        return;

    }

    FILE *fp = fopen(nomArq, "a");

    if (fp == NULL) {

        perror("Erro ao abrir o arquivo para gravação");
        return;

    }

    char txt[TMBUF];

    printf("Digite o texto a gravar: ");

    scanf(" %255[^\n]", txt); // Aceita qualquer char exceto '\n', max 255

    fprintf(fp, "%s\n", txt);

    printf("Linha gravada com sucesso em '%s'!\n\n", nomArq);

    fclose(fp);

}

void lerArq() {

    if (strlen(nomArq) == 0) {

        printf("Nenhum arquivo definido! Escolha a opção 1 primeiro.\n");
        return;

    }

    FILE *fp = fopen(nomArq, "r");

    if (fp == NULL) {

        printf("Arquivo '%s' não encontrado ou não pode ser aberto.\n\n", nomArq);

        return;

    }

    char buf[TMBUF];

    int lin = 1;

    printf("\n----------------------- Conteúdo de '%s' -----------------------\n", nomArq);

    while (fscanf(fp, " %255[^\n]", buf) == 1) {

        printf("[%d] %s\n", lin++, buf);

    }

    printf("-------------------------------------------------------------------------\n\n");

    fclose(fp);

}

struct No {

    int pos;
    char Linha[TMBUF];

    struct No *ProxNo;

};

No *ListaUniversal = NULL;

void InserirMusica() {

    No *MostrarLista = ListaUniversal;

    while (MostrarLista != NULL) {

        printf("[Posição: %d] %s", MostrarLista->pos, MostrarLista->Linha);

        MostrarLista = MostrarLista->ProxNo;

    }

    free(MostrarLista);
    MostrarLista = NULL;

    No *NovaMusica = (No*)malloc(sizeof(No));

    //ID único, título, Artista, Ano, Álbum, Tipo de Versão e ID_Original

    printf("\n\nInforme os dados que deseja inserir em sua Playlist.\nObs: Deve conter os seguintes atributos [ ID Música, Título, Artista/Banda, Ano de lançamento, Álbum, Tipo da versão e ID da Música Original].\nEx : (001;Retoque de Batom;Banda G10;2021;Single;Original;001).\n\n");
    scanf(" %255[^\n]", NovaMusica->Linha);
    strcat(NovaMusica->Linha, "\n");

    FILE *Append = fopen(nomPlaylist, "a");

    if (Append == NULL) {

        perror("Erro ao abrir o arquivo para gravação");
        return;

    }

    fprintf(Append, "%s\n", NovaMusica->Linha);

    printf("Linha gravada com sucesso em '%s'!\n\n", nomPlaylist);

    fclose(Append);

    printf("\nInforme qual posição deseja inserir a música. \n");
    scanf("%d", &NovaMusica->pos);

    NovaMusica->ProxNo = NULL;

    No *Aux1 = ListaUniversal;

    if (NovaMusica->pos == Aux1->pos) {    

        NovaMusica->ProxNo = Aux1;

        while(Aux1 != NULL) {

            Aux1->pos++;
            Aux1 = Aux1->ProxNo;

        }

        ListaUniversal = NovaMusica;
              
    }

    else {

        Aux1 = ListaUniversal;

        while(Aux1->ProxNo != NULL) {

            if(Aux1->ProxNo != NULL) {
        
                Aux1 = Aux1->ProxNo;

            }

        }

        //Aux1 ficou com a última posição da lista.

        if (NovaMusica->pos > Aux1->pos) {

            int pos = Aux1->pos;

            pos++;

            NovaMusica->pos = pos;

            printf("Inserido na última posição.\n");

            Aux1->ProxNo = NovaMusica;

        } 

        else {

            Aux1 = ListaUniversal;
            No *Aux2 = ListaUniversal;

            while (Aux1->pos != NovaMusica->pos - 1) {

                Aux1 = Aux1->ProxNo;

            }

            while (Aux2->pos != NovaMusica->pos) {
                
                Aux2 = Aux2->ProxNo;

            }
            
            Aux1->ProxNo = NovaMusica;
            NovaMusica->ProxNo = Aux2;

        }        

    }//Fim do Else.

    int i = 1;

    No *Aux = ListaUniversal;

    while (Aux != NULL) {

        Aux->pos = i++;

        Aux = Aux->ProxNo;

    }

    Aux = ListaUniversal;

    printf("\nLista após inserir a música: \n\n");

    do {

        printf("Posição [ %d ] %s", Aux->pos, Aux->Linha);

        Aux = Aux->ProxNo;

    }

    while(Aux != NULL);
    

    sleep(7);

    system("Clear");

    Aux = NULL;
    Aux1 = NULL;

    }

void RemoverMusica() {

    if (ListaUniversal == NULL) {

        printf("Playlist vazia!\n");

    }

    No *InicioFila = ListaUniversal;

    printf("\n----------------------- Conteúdo de '%s' -----------------------\n", nomPlaylist);

    while (InicioFila != NULL) {

        printf("Posição [ %d ] %s", InicioFila->pos, InicioFila->Linha);

        InicioFila = InicioFila->ProxNo;

    }

    printf("-------------------------------------------------------------------------\n\n");

    int pos;

    printf("Informe a posição que deseja remover a música.\n");
    scanf("%d", &pos);

    if (pos <= 0) {

        printf("Posição inválida!\n");
        return;

    }

    if (pos == 1) {

        No *remover = ListaUniversal;

        ListaUniversal = ListaUniversal->ProxNo;

        free(remover);

    } else {

        No *anterior = ListaUniversal;

        while (anterior != NULL && anterior->pos != pos - 1) {

            anterior = anterior->ProxNo;

        }

        if (anterior == NULL || anterior->ProxNo == NULL) {

            printf("Posição inválida!\n");

        }

        No *remover = anterior->ProxNo;

        anterior->ProxNo = remover->ProxNo;

        free(remover);

    }

    No *Aux = ListaUniversal;
    
    int i = 1;

    while (Aux != NULL) {

        Aux->pos = i++;

        Aux = Aux->ProxNo;

    }

    printf("Música da posição [ %d ] removida!\n", pos);

    InicioFila = ListaUniversal;

    printf("\n----------------------- Conteúdo após a remoção -----------------------\n");

    while (InicioFila != NULL) {

        printf("Posição [ %d ] %s", InicioFila->pos, InicioFila->Linha);

        InicioFila = InicioFila->ProxNo;

    }

    printf("-------------------------------------------------------------------------\n\n");

    sleep(7);

    system("clear");

}

void MoverMusica() {

    if (ListaUniversal == NULL) {

        printf("Playlist vazia!\n");

    }

    else {

        No *Exibir = ListaUniversal;

        printf("\n----------------------- Conteúdo de '%s' -----------------------\n", nomPlaylist);

        while (Exibir != NULL) {

        printf("Posição [ %d ] %s", Exibir->pos, Exibir->Linha);

        Exibir = Exibir->ProxNo;

        }

        printf("-------------------------------------------------------------------------\n\n");

        //

        int pos;

        printf("Informe a POSIÇÃO da música que deseja mover.\n");
        scanf("%d", &pos);

        No *MusicaArmazenada = ListaUniversal;
        
        No *MusicaAnterior = ListaUniversal;

        if (pos == 1) {

            ListaUniversal = MusicaArmazenada->ProxNo;

            //Ficou Armazenado o primeiro da lista.
            MusicaArmazenada->ProxNo = NULL;

        }

        else {

            while (MusicaAnterior != NULL && MusicaAnterior->pos != pos - 1) {

                MusicaAnterior = MusicaAnterior->ProxNo;

            }

            if (MusicaAnterior == NULL || MusicaAnterior->ProxNo == NULL) {

                printf("Posição inválida!\n");

            }

            MusicaArmazenada = MusicaAnterior->ProxNo;

            MusicaAnterior->ProxNo = MusicaArmazenada->ProxNo;

            MusicaArmazenada->ProxNo = NULL;

        }

        Exibir = ListaUniversal;

        int i = 1;

        while (Exibir != NULL) {
            
            Exibir->pos = i++;

            Exibir = Exibir->ProxNo;

        }

        Exibir = ListaUniversal;

        printf("\n----------------------- Lista Para Inserir -----------------------\n");

        while (Exibir != NULL) {

        printf("Posição [ %d ] %s", Exibir->pos, Exibir->Linha);

        Exibir = Exibir->ProxNo;

        }

        printf("-------------------------------------------------------------------------\n\n");

        printf("Musica em espera:  %s ", MusicaArmazenada->Linha);

        //Lembrando que a MusicaArmazenada possuí o nó da música.

        pos = 1000;

        printf("\n\nInforme a POSIÇÃO para inserir a música.\n");
        scanf("%d", &pos);

        MusicaAnterior = ListaUniversal;

        if (pos == 1) {

            MusicaArmazenada->ProxNo = ListaUniversal;

            ListaUniversal = MusicaArmazenada;

        }

        else {

            while (MusicaAnterior != NULL && MusicaAnterior->pos != pos - 1) {

                MusicaAnterior = MusicaAnterior->ProxNo;

            }

            if (MusicaAnterior == NULL || MusicaAnterior->ProxNo == NULL) {

                printf("Posição inválida!\n");

            }
            
            MusicaArmazenada->ProxNo = MusicaAnterior->ProxNo;

            MusicaAnterior->ProxNo = MusicaArmazenada;

        }

        Exibir = ListaUniversal;

        i = 1;

        while (Exibir != NULL) {
            
            Exibir->pos = i++;

            Exibir = Exibir->ProxNo;

        }
        
        Exibir = ListaUniversal;

        printf("\n----------------------- Lista Após a Inserção -----------------------\n");

        while (Exibir != NULL) {

        printf("Posição [ %d ] %s", Exibir->pos, Exibir->Linha);

        Exibir = Exibir->ProxNo;

        }

        printf("-------------------------------------------------------------------------\n\n");

        sleep(7);

        system("clear");

        Exibir = NULL;
        MusicaAnterior = NULL;
        MusicaArmazenada = NULL;

    }//Fim do Primeiro Else.

}

void BuscarArtista() {

    if (ListaUniversal == NULL) {

        printf("Playlist vazia!\n");
        return;

    }

    char artistaBusca[100];

    printf("Informe o nome do artista/banda: ");
    scanf(" %99[^\n]", artistaBusca);

    No *Aux = ListaUniversal;

    int encontrou = 0;

    printf("\nFaixas encontradas:\n\n");

    while (Aux != NULL) {

        char copiaLinha[256];

        strcpy(copiaLinha, Aux->Linha);

        strtok(copiaLinha, ";");  //Fica armazenado o ID

        strtok(NULL, ";"); //Fica armazenado o título.

        char *artista = strtok(NULL, ";"); //Fica armazenado o artista/ Banda.

        if (artista != NULL && strcmp(artista, artistaBusca) == 0) {

            printf("Posição [%d] %s", Aux->pos, Aux->Linha);

            encontrou = 1;

        }

        Aux = Aux->ProxNo;

    }

    if (!encontrou) {

        printf("Nenhuma música encontrada para '%s'.\n", artistaBusca);

    }

    sleep(7);

    system("clear");

}

void EmbaralharPlaylist() {

    if (ListaUniversal == NULL || ListaUniversal->ProxNo == NULL) {

        printf("Não há músicas suficientes para embaralhar.\n");
        return;

    }

    int tamanho = 0;

    No *Aux = ListaUniversal;

    while (Aux != NULL) {

        tamanho++;

        Aux = Aux->ProxNo;

    }

    srand(time(NULL));

    for (int i = 0; i < tamanho * 2; i++) {

        int p1 = rand() % tamanho + 1;

        int p2 = rand() % tamanho + 1;

        if (p1 == p2)

            continue;

        No *N1 = ListaUniversal;

        No *N2 = ListaUniversal;

        while (N1 != NULL && N1->pos != p1) {

            N1 = N1->ProxNo;

        }

        while (N2 != NULL && N2->pos != p2) {

            N2 = N2->ProxNo;

        }

        if (N1 != NULL && N2 != NULL) {

            char linhaTemp[256];
            strcpy(linhaTemp, N1->Linha);

            strcpy(N1->Linha, N2->Linha);
            strcpy(N2->Linha, linhaTemp);

        }

    }

    printf("\nPlaylist embaralhada com sucesso!\n");

    Aux = ListaUniversal;

    while (Aux != NULL) {

        printf("Posição [%d] %s", Aux->pos, Aux->Linha);

        Aux = Aux->ProxNo;

    }

    sleep(7);

    system("clear");

}

void OrdenarAlfabeticamente() {

    if (ListaUniversal == NULL || ListaUniversal->ProxNo == NULL) {

        printf("Não há músicas suficientes para ordenar.\n");
        return;

    }

    int trocou;

    do {

        trocou = 0;

        No *Atual = ListaUniversal;

        while (Atual->ProxNo != NULL) {

            char Linha1[256];

            char Linha2[256];

            strcpy(Linha1, Atual->Linha);

            strcpy(Linha2, Atual->ProxNo->Linha);

            strtok(Linha1, ";");

            char *Titulo1 = strtok(NULL, ";");

            strtok(Linha2, ";");

            char *Titulo2 = strtok(NULL, ";");

            if (Titulo1 != NULL && Titulo2 != NULL && strcmp(Titulo1, Titulo2) > 0) {

                char Temp[256];

                strcpy(Temp, Atual->Linha);

                strcpy(Atual->Linha, Atual->ProxNo->Linha);

                strcpy(Atual->ProxNo->Linha, Temp);

                trocou = 1;
            }

            Atual = Atual->ProxNo;

        }

    } 
    
    while (trocou);

    printf("\nPlaylist ordenada alfabeticamente:\n\n");

    No *Exibir = ListaUniversal;

    while (Exibir != NULL) {

        printf("Posição [ %d ] %s", Exibir->pos, Exibir->Linha);

        Exibir = Exibir->ProxNo;

    }

    sleep(7);

    system("clear");

}

void criarPlaylist() {

    FILE *Original = fopen(nomArq, "r");

    lbt();

    printf("Defina o nome da playlist: \n");
    scanf(" %127[^\n]", nomPlaylist); 
    
    FILE *Copia = fopen(nomPlaylist, "w+");

    char Linha[TMBUF];

    if(Original == NULL || Copia == NULL) {

        printf("Erro ao abrir os arquivos!!\n");

    }

    while (fgets(Linha, sizeof(Linha),Original) != NULL) {

        fputs(Linha,Copia);

    }

    fclose(Copia);

    fclose(Original);

    FILE *CopiaPlaylist = fopen(nomPlaylist, "r");

    No *Inicio = ListaUniversal;

    No *Fim = NULL;

    char buf[TMBUF];

    int pos = 1;

    while(fgets(buf, sizeof(buf), CopiaPlaylist) != NULL) {

        No *GravarLinha = (No*)malloc(sizeof(No));

        strcpy(GravarLinha->Linha, buf);

        GravarLinha->pos = pos;

        pos++;

        if(Inicio == NULL) {
                        
            Inicio = GravarLinha;

            Fim = GravarLinha;

        }

        else {

            Fim->ProxNo = GravarLinha;

            Fim = GravarLinha;

        }
                    
    }

    ListaUniversal = Inicio;

    fclose(CopiaPlaylist);

    system("Clear");

    int op = 100;

    while (op != 7) {

        No *MostrarPlaylist = ListaUniversal;

        printf("\n----------------------- Conteúdo de '%s' -----------------------\n\n", nomPlaylist);

        while (MostrarPlaylist != NULL) {

            printf("Posição [ %d ] %s", MostrarPlaylist->pos, MostrarPlaylist->Linha);

            MostrarPlaylist = MostrarPlaylist->ProxNo;

        }
        

        printf("\n\n**** PLAYLIST '%s'****\n\n", nomPlaylist);
        printf("< 1 > Inserir Música\n");
        printf("< 2 > Remover Música\n");
        printf("< 3 > Mover Música\n");
        printf("< 4 > Buscar Música\n");
        printf("< 5 > Embaralhar Playlist\n");
        printf("< 6 > Ordenar Músicas\n");
        printf("< 7 > Sair\n");

        scanf("%d", &op);


        switch(op) {

            case 1: {

                system("Clear");

                InserirMusica();

            }

            break;

            case 2: {

                system("Clear");

                RemoverMusica();

            }

            break;

            case 3: {

                system("Clear");

                MoverMusica();

            }

            break;

            case 4: {

                system("Clear");

                BuscarArtista();

            }

            break;

            case 5: {

                system("Clear");

                EmbaralharPlaylist();

            }

            break;

            case 6: {

                system("Clear");

                OrdenarAlfabeticamente();

            }

            break;

            default:

                printf("Opção errada!! Tente novamente.\n");

            break;

       }//Fim do Switch.

    }

}

int main() {

    int op;

    do {

        system("clear");

        printf("**** MENU PRINCIPAL ****\n");

        if (strlen(nomArq) > 0) {

            printf("Arquivo atual: '%s'\n", nomArq);
            printf("< 1 > Definir nome do arquivo\n");
            printf("< 2 > Gravar linha no arquivo (append)\n");
            printf("< 3 > Ler conteúdo do arquivo\n");
            printf("< 4 > Criar Playlist\n");
            printf("< 5 > Sair do programa\n");
            printf("Escolha sua Opção: ");
            scanf("%d", &op);

            system("Clear");

            switch (op) {

                case 1:

                    definirArq();

                break;

                case 2:

                    gravarLinha();

                    sleep(5);

                break;

                case 3:

                    lerArq();

                    sleep(5);

                break;

                case 4: 

                    criarPlaylist();

                break;

                case 5:

                    printf("Saindo do Programa! Tchau...\n");

                break;

                default:

                printf("ERRO! Informe 1, 2, 3 ou 4\n\n");

                break;

            }


        }

    }

    while (op != 5);

}
