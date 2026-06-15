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

// INCLUSÃO DE TODAS AS BIBLIOTECAS NECESSÁRIAS.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// DEFININDO TMBUF E TMNOME, PARA ESTABELECER UM PADRÃO DE TAMANHO.
#define TMBUF 256
#define TMNOME 128

// CRIANDO DUAS VARIÁVEIS GLOBAIS PARA PERCORRER EM DIVERSAS FUNÇÕES, 'nomArq' RESPONSÁVEL POR GUARDAR O ARQUIVO .TXT JÁ
// O ARQUIVO nomPlaylist VAI SER O ARQUIVO CLONADO PARA MODIFICAÇÃO DA PLAYLIST.
char nomArq[TMNOME] = "Nenhum";
char nomPlaylist[TMNOME] = "Nenhum";

// LIMPA BUFFER PADRÃO.
void lbt() {

    char c;

    while((c = getchar()) != '\n' && c != EOF);

}

// ESSA FUNÇÃO VAI REGISTRAR O ARQUIVO QUE O USUÁRIO QUER ACESSAR.
void definirArq() {

    printf("Digite o nome do arquivo (ex: dados.txt): ");

    scanf(" %127[^\n]", nomArq);

    // FUNÇÃO STRLEN VAI CONTAR TODAS A LETRAS DO NOME DO ARQUIVO, NO CASO 'nomArq'.
    if (strlen(nomArq) == 0) {

        printf("Nome inválido! Usando 'arquivo.txt' como padrão.\n");
        strcpy(nomArq, "arquivo.txt");

    }

    printf("Arquivo definido: '%s'\n\n", nomArq);

}

// ESSA É UMA FUNÇÃO ESPECÍFICA, POIS GRAVA UMA LINHA QUE O USUÁRIO DIGITAR NA ÚLTIMA POSIÇÃO DO ARQUIVO .TXT.
void gravarLinha() {

    if (strlen(nomArq) == 0) {

        printf("Nenhum arquivo definido! Escolha a opção 1 primeiro.\n");

        return;

    }

    // A FUNÇÃO FILE FAZ COM QUE O PROGRAMA ENTENDE QUE É UM ARQUIVO A SER ABERTO, FOPEN(nomArq, "a") ABRE O ARQUIVO nomArq, 
    // E A LETRA "a" SIGNIFICA APPEND NA QUAL INSERE O TEXTO NA ÚLTIMA POSIÇÃO.
    // "a" APPEND, INSERE NA ÚLTIMA POSIÇÃO DO .TXT.
    // "w" WRITE, SE EXISTIR O ARQUIVO ELE APAGA E CRIA UM NOVO.
    // "r" READ, SOMENTE LEITURA DO ARQUIVO .TXT.
    FILE *fp = fopen(nomArq, "a");

    if (fp == NULL) {

        perror("Erro ao abrir o arquivo para gravação");
        return;

    }

    char txt[TMBUF];

    printf("Digite o texto a gravar: ");

    // Aceita qualquer char exceto '\n', max 255
    scanf(" %255[^\n]", txt); 

    // VAI GRAVAR O TEXTO QUE FOI ESCRITO "txt" NO ARQUIVO "fp".
    fprintf(fp, "%s\n", txt);

    printf("Linha gravada com sucesso em '%s'!\n\n", nomArq);

    // FECHA O ARQUIVO QUE FOI ABERTO LÁ EM CIMA.
    fclose(fp);

}

// VAI ABRIR O ARQUIVO DEFINIDO E MOSTRAR PARA O USUÁRIO.
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

    // VAI IMPRIMIR TODO O CONTEÚDO DO ARQUIVO.
    printf("\n----------------------- Conteúdo de '%s' -----------------------\n", nomArq);

    while (fscanf(fp, " %255[^\n]", buf) == 1) {

        printf("[%d] %s\n", lin++, buf);

    }

    printf("-------------------------------------------------------------------------\n\n");

    fclose(fp);

}

// COMO NO ENUNCIADO FOI SOLICITADO A UTILIZAÇÃO DE LISTA SIMPLES, FOI CRIADO UMA ESTRUTURA DE DADOS CONTENDO A POSIÇÃO,
// A LINHA A SER ESCRITA E O PRÓXIMO NÓ, NO CASO A PRÓXIMA MÚSICA.
struct No {

    int pos;
    char Linha[TMBUF];

    struct No *ProxNo;

};

// PODEMOS SE DEFINIR COMO A RAIZ DA "Playlist", SE PERDER ESSE PRIMEIRO NÓ, NÃO TERÁ ACESSO NAS PRÓXIMAS MÚSICAS.
No *ListaUniversal = NULL;

// APARTIR DESSE PONTO COMEÇAREMOS A MANIPULAÇÃO DA LISTA.

// AO SER ACIONADA PELO USUÁRIO, VAI SOLICITAR O MÚSICA E A POSIÇÃO QUE DESEJA INSERIR NA PLAYLIST.
void InserirMusica() {

    // DECLARANDO A ESTRUTURA RAIZ.
    No *MostrarLista = ListaUniversal;

    // MOSTRA A LISTA COMPLETA.
    while (MostrarLista != NULL) {

        printf("[Posição: %d] %s", MostrarLista->pos, MostrarLista->Linha);

        MostrarLista = MostrarLista->ProxNo;

    }

    // LIBERA A ESTRUTURA PARA DESOCUPAR MEMÓRIA DO SISTEMA.
    free(MostrarLista);
    MostrarLista = NULL;

    No *NovaMusica = (No*)malloc(sizeof(No));

    //ID único, título, Artista, Ano, Álbum, Tipo de Versão e ID_Original

    printf("\n\nInforme os dados que deseja inserir em sua Playlist.\nObs: Deve conter os seguintes atributos [ ID Música, Título, Artista/Banda, Ano de lançamento, Álbum, Tipo da versão e ID da Música Original].\nEx : (001;Retoque de Batom;Banda G10;2021;Single;Original;001).\n\n");
    scanf(" %255[^\n]", NovaMusica->Linha);
    // ADICIONA UMA QUEBRA DE LINHA NO FINAL DA MÚSICA INSERIDA.
    strcat(NovaMusica->Linha, "\n");

    FILE *Append = fopen(nomPlaylist, "a");

    if (Append == NULL) {

        perror("Erro ao abrir o arquivo para gravação");
        return;

    }

    // GRAVA A MÚSICA NA ÚLTIMA POSIÇÃO DO ARQUIVO .TXT.
    fprintf(Append, "%s\n", NovaMusica->Linha);

    printf("Linha gravada com sucesso em '%s'!\n\n", nomPlaylist);

    fclose(Append);

    printf("\nInforme qual posição deseja inserir a música. \n");
    scanf("%d", &NovaMusica->pos);

    NovaMusica->ProxNo = NULL;

    // UTILIZA DA RAIZ PARA ACESSAR A PLAYLIST.
    No *Aux1 = ListaUniversal;

    // SE O USUÁRIO QUER INSERIR NA PRIMEIRA POSIÇÃO.

    // PODEMOS RE-FORMULAR PARA, if (NovaMusica->pos == 1).
    if (NovaMusica->pos == Aux1->pos) {    

        NovaMusica->ProxNo = Aux1;

        while(Aux1 != NULL) {

            Aux1->pos++;
            Aux1 = Aux1->ProxNo;

        }

        // DEFINE COMO O A RAIZ DA PLAYLIST A NOVA MÚSICA.
        ListaUniversal = NovaMusica;
              
    }

    else {

        Aux1 = ListaUniversal;

        // VAI CRIAR UM NÓ COM A ÚLTIMA POSIÇÃO DA PLAYLIST.
        while(Aux1->ProxNo != NULL) {

            if(Aux1->ProxNo != NULL) {
        
                Aux1 = Aux1->ProxNo;

            }

        }

        // SE O USUÁRIO DIGITOU A ÚLTIMA POSIÇÃO OU MAIOR, É INSERIDO NA ÚLTIMA POSIÇÃO DA PLAYLIST.
        if (NovaMusica->pos > Aux1->pos) {

            int pos = Aux1->pos;

            pos++;

            NovaMusica->pos = pos;

            printf("Inserido na última posição.\n");

            Aux1->ProxNo = NovaMusica;

        } 

        // SE A POSIÇÃO FICAR ENTRE O PRIMEIRO E O ÚLTIMA DA PLAYLIST, VAI UTILIZAR ESSE FORMATO PARA MANIPULAR A PLAYLIST.
        // LEMBRANDO QUE A UTILIZAÇÃO DE DOIS AUXILIAR É DESNECESSÁRIO, SOMENTE UM JÁ FAZIA A FUNÇÃO.
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

    }

    // VAI RE-FORMULAR AS POSIÇÕES DAS MÚSICAS DA PLAYLIST.
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
    
    //DEFINIDO COMO PADRÃO, ESPERA 7 SEGUNDOS PARA LEITURA E LIMPA O SISTEMA.
    sleep(7);

    system("Clear");

    Aux = NULL;
    Aux1 = NULL;

    }

// AO SER ACIONADO O USUÁRIO TEM O PODER DE EXCLUIR A MÚSICA DA PLAYLIST.
void RemoverMusica() {

    // SE NÃO EXISTIR CONTEÚDO NA PLAYLIST.
    if (ListaUniversal == NULL) {

        printf("Playlist vazia!\n");

    }

    //DEFINIDO A RAIZ PARA A EXIBIÇÃO DO CONTEÚDO DA PLAYLIST.
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

    // SE INFORMAR UMA POSIÇÃO MENOR QUE O PERMITIDO.
    if (pos <= 0) {

        printf("Posição inválida!\n");
        return;

    }

    // SE INFORMAR A POSIÇÃO IGUAL A UM.
    if (pos == 1) {

        No *remover = ListaUniversal;

        ListaUniversal = ListaUniversal->ProxNo;

        free(remover);

    } 
    
    // AQUI MUDAMOS A FORMA DE LOCALIZAR O NÓ ANTERIOR.
    else {

        No *anterior = ListaUniversal;

        // USANDO UM LAÇO DE REPETIÇÃO PARA LOCALIZAR O NÓ ANTERIOR DO NÓ QUE O USUÁRIO DESEJA REMOVER.
        while (anterior != NULL && anterior->pos != pos - 1) {

            anterior = anterior->ProxNo;

        }

        // SE O NÓ ANTERIOR FOR IGUAL A NULO OU O NÓ ANTERIOR->PRÓXIMO NÓ FOR IGUAL A NULO.
        if (anterior == NULL || anterior->ProxNo == NULL) {

            printf("Posição inválida!\n");

        }

        // UMA PARTE INTERESSANTE, CRIA UM NÓ QUE RECEBE O NOÓ DA MÚSICA A SER EXCLUIDA , LOGO EM SEGUIDA LIGA O 
        // NÓ ANTES DO NÓ A SER EXCLUIDO PARA O PRÓXIMO NÓ A SER EXCLUIDO.

        // NÓ1 -------> NÓ3

        //       NÓ2 --- NÓ EXCLUÍDO.

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

// MOVE A MÚSICA ESCOLHIDA PARA A POSIÇÃO DESEJADA.
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

            // MUSICAARMAZENDA FICA GUARDADO A MÚSICA QUE O USUÁRIO ESCOLHEU PARA MUDAR DE POSIÇÃO.
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

        // AQUI O USUÁRIO VAI DEFINIR A POSIÇÃO PARA COLOCAR A MÚSICA NA PLAYLIST.
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
            
            // MUSICAARMAZENADA ------> NO2
            MusicaArmazenada->ProxNo = MusicaAnterior->ProxNo;

            // NO1 ------> MUSICAARMAZENDA ------> NO2
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

// AQUI O USUÁRIO PODE LOCALIZAR QUANTAS VEZES A BANDA OU ARTISTA APARECE NA PLAYLIST.
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

        //VERIFICA SE O ARTISTA DA LINHA É O MESMO QUE O USUÁRIO PROCURA, SE FOR, CONTABILIZA NO CONTADOR.
        if (artista != NULL && strcasecmp(artista, artistaBusca) == 0) {

            printf("Posição [%d] %s", Aux->pos, Aux->Linha);

            encontrou = 1;

        }

        Aux = Aux->ProxNo;

    }

    // SE NÃO ENCONTRAR EM NENHUMA POSIÇÃO, ELE AVISA.
    if (!encontrou) {

        printf("Nenhuma música encontrada para '%s'.\n", artistaBusca);

    }

    sleep(7);

    system("clear");

}

// VAI EMBARALHAR TODA A PLAYLIST DE FORMA DEFINITIVA.
void EmbaralharPlaylist() {

    // SE NÃO EXISTIR MÚSICAS OU SOMENTE UMA MÚSICA, OCORRE ERRO.
    if (ListaUniversal == NULL || ListaUniversal->ProxNo == NULL) {

        printf("Não há músicas suficientes para embaralhar.\n");
        return;

    }

    int tamanho = 0;

    No *Aux = ListaUniversal;

    // CONTABILIZA TODAS AS MÚSICAS.
    while (Aux != NULL) {

        tamanho++;

        Aux = Aux->ProxNo;

    }

    // GERA NÚMEROS DE FORMA ALEATÓRIA TODA VEZ QUE FOR INICIADO.
    srand(time(NULL));

    // RECURSIVIDADE ATÉ QUE I ALCANCE 2 VEZES O TAMANHO.
    for (int i = 0; i < tamanho * 2; i++) {

        // P1 E P2 VAI SER ATRIBUIDO UM VALOR ENTRE UM E O TAMANHO.
        int p1 = rand() % tamanho + 1;

        int p2 = rand() % tamanho + 1;

        // SE ACASO CAIR OS DOIS NA MESMA POSIÇÃO, PULA PARA O PRÓXIMO REPETIÇÃO DO LAÇO.
        if (p1 == p2)

            continue;

        No *N1 = ListaUniversal;

        No *N2 = ListaUniversal;

        // ENCONTRA NA PLAYLIST A POSIÇÃO P1.
        while (N1 != NULL && N1->pos != p1) {

            N1 = N1->ProxNo;

        }

        // ENCONTRA NA PLAYLIST A POSIÇÃO P2.
        while (N2 != NULL && N2->pos != p2) {

            N2 = N2->ProxNo;

        }

        // AQUI VAI COLOCAR A LINHA DO P1 NA P2 E P2 NA LINHA DO P1.
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

// AQUI OCORRE A ORGANIZAÇÃO DE FORMA ALFABÉTICA.
void OrdenarAlfabeticamente() {

    if (ListaUniversal == NULL || ListaUniversal->ProxNo == NULL) {

        printf("Não há músicas suficientes para ordenar.\n");
        return;

    }

    int trocou;

    do {

        trocou = 0;

        No *Atual = ListaUniversal;

        // PRECORRE TODO OS NÓ ATÉ O PRÓXIMO NÓ SER NULO.
        while (Atual->ProxNo != NULL) {

            // ARMAZENA AS LINHAS.
            char Linha1[256];

            char Linha2[256];

            // COPIA NA LINHA1 A LINHA ATUAL.
            strcpy(Linha1, Atual->Linha);

            // COPIA NA LINHA2 O PRÓXIMO NÓ DA LINHA ATUAL.
            strcpy(Linha2, Atual->ProxNo->Linha);

            // VAI IGNORAR O ID.
            strtok(Linha1, ";");

            // VAI USAR SOMENTE O TÍTULO DA MÚSICA.
            char *Titulo1 = strtok(NULL, ";");

            strtok(Linha2, ";");

            char *Titulo2 = strtok(NULL, ";");

            // A FUNÇÃO STRCMP VAI COMPARA DE FORMA ALFABÉTICA AS DUAS STRINGS.
            if (Titulo1 != NULL && Titulo2 != NULL && strcmp(Titulo1, Titulo2) > 0) {

                // LINHA PARA ARMAZENAMENTO.
                char Temp[256];

                // COPIA A LINHA ATUAL NO CASO A LINHA QUE POSSUI O A LETRA INICIAL DEPOIS DA LINHA 2, EX: ZORRO --> LINHA ATUAL, ABACAXI --> LINHA2. 
                strcpy(Temp, Atual->Linha);

                // A LINHA ATUAL RECEBE LINHA2.
                strcpy(Atual->Linha, Atual->ProxNo->Linha);

                // LINHA2 RECEBE A LINHA ATUAL --> ZORRO.
                strcpy(Atual->ProxNo->Linha, Temp);

                // COMUM EM BUUBLE SORT PARA SABER SE ESTÁ ORDENADA OU NÃO.
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

// AQUI É CRIADO A BASE DA PLAYLIST, AONDE É COPIADO DA LISTA ORIGINAL PARA UTILIZAR DE MANIPULAÇÃO.
void criarPlaylist() {

    // ABRE O ARQUIVO PARA LEITURA.
    FILE *Original = fopen(nomArq, "r");

    lbt();

    printf("Defina o nome da playlist: \n");
    scanf(" %127[^\n]", nomPlaylist); 
    
    FILE *Copia = fopen(nomPlaylist, "w+");

    char Linha[TMBUF];

    if(Original == NULL || Copia == NULL) {

        printf("Erro ao abrir os arquivos!!\n");

    }

    // COPIA TODA O ARQUIVO .TXT PARA O NOVO.
    while (fgets(Linha, sizeof(Linha),Original) != NULL) {

        fputs(Linha,Copia);

    }

    fclose(Copia);

    fclose(Original);

    // ABRE O ARQUIVO COPIADO.
    FILE *CopiaPlaylist = fopen(nomPlaylist, "r");

    // CRIA A "RAIZ".
    No *Inicio = ListaUniversal;

    No *Fim = NULL;

    char buf[TMBUF];

    int pos = 1;

    // VAI GRAVAR CADA LINHA DA CÓPIA EM UMA ESTRUTURA.
    while(fgets(buf, sizeof(buf), CopiaPlaylist) != NULL) {

        // DEFINE A ESTRUTURA PARA MANIPULAÇÃO.
        No *GravarLinha = (No*)malloc(sizeof(No));

        // JÁ DEFINE O PRÓXIMO NÓ COMO NULO PARA EVITA LIXO DE MEMÓRIA.
        GravarLinha->ProxNo = NULL;

        //GRAVA LINHA.
        strcpy(GravarLinha->Linha, buf);
    
        // GRAVA A POSIÇÃO.
        GravarLinha->pos = pos;

        // AUMENTA A POSIÇÃO.
        pos++;

        // SE FOR O PRIMERO DA LISTA.
        if(Inicio == NULL) {
                        
            Inicio = GravarLinha;

            Fim = GravarLinha;

        }

        // SE NÃO FOR O PRIMEIRO DA LISTA.
        else {

            // O PRÓXIMO NÓ DO FIM RECEBE O NÓ QUE FOI GRAVADO.
            Fim->ProxNo = GravarLinha;

            // O FIM RECEBE O NOVO NÓ GRAVADO.
            Fim = GravarLinha;

        }
                    
    }

    // A RAIZ RECEBE O PRIMEIRO DA LISTA/PLAYLIST CRIADA.
    ListaUniversal = Inicio;

    fclose(CopiaPlaylist);

    system("Clear");

    int op = 100;

    // AGORA O USUÁRIO EM SUA TELA DEVERÁ ESCOLHER OQUE FAZER COM SUA PLAYLIST.
    while (op != 7) {

        // MOSTRA PARA O USUÁRIO AS MÚSICAS.
        No *MostrarPlaylist = ListaUniversal;

        printf("\n----------------------- Conteúdo de '%s' -----------------------\n\n", nomPlaylist);

        while (MostrarPlaylist != NULL) {

            printf("Posição [ %d ] %s", MostrarPlaylist->pos, MostrarPlaylist->Linha);

            MostrarPlaylist = MostrarPlaylist->ProxNo;

        }
        
        // OPÇÕES DISPONÍVEIS.
        printf("\n\n**** PLAYLIST '%s'****\n\n", nomPlaylist);
        printf("< 1 > Inserir Música\n");
        printf("< 2 > Remover Música\n");
        printf("< 3 > Mover Música\n");
        printf("< 4 > Buscar Música\n");
        printf("< 5 > Embaralhar Playlist\n");
        printf("< 6 > Ordenar Músicas\n");
        printf("< 7 > Sair\n");

        // OPÇÃO ESCOLHIDA PELO USUÁRIO.
        scanf("%d", &op);


        switch(op) {

            case 1: {

                // LIMPA A TELA E CHAMA A FUNÇÃO.
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

// MENU PRINCIPAL, ESCOLHE O ARQUIVO PRÉ DEFINIDO NO CASO MUSICAS1 E MUSICAS2, GRAVAR LINHA NO ARQUIVO ORIGINAL, 
// LER O CONTEÚDO ORIGINAL, CRIAR A PLAYLIST COMO CÓPIA E SAIR DO APLICATIVO DE MÚSICA.
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