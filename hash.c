// Exemplo de uso da funcao SHA256 da biblioteca Open SSL
//  Este código requer a instalação do pacote libssl-dev
//  Para compilar este código use gcc hash.c -lcrypto

#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h"    //arquivo de definição necessário função SHA256
#include <string.h>
#include "mtwister.h"
#include "mtwister.c"
#include <stdlib.h>

#define HASH_TABLE_SIZE 60000

//-------------------------------------------------------------------------------

typedef struct BlocoNaoMinerado
{
  unsigned int numero;
  unsigned int nonce;
  unsigned char data[184];
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
} BlocoNaoMinerado;

//-------------------------------------------------------------------------------

typedef struct BlocoMinerado
{
  BlocoNaoMinerado bloco;
  unsigned char hash[SHA256_DIGEST_LENGTH];
} BlocoMinerado;

//-------------------------------------------------------------------------------

typedef struct TIndiceMinerador{
  unsigned int minerador;
  BlocoMinerado blocoDoMinerador;
}TIndiceMinerador;

//-------------------------------------------------------------------------------
// lista que vai conter os enderecos que possuem BTC
typedef struct listaBTC
{
  unsigned int endereco;
  struct listaBTC *next;
} listaBTC;

typedef struct TBlocoETransacoes{
  unsigned int qtdTransacoes;
  BlocoMinerado bloco;
}TBlocoETransacoes;

// Funcao para criar um no na lista dos enderecos que tem BTC
listaBTC *criaNo(unsigned int endereco)
{
  listaBTC *novoEndereco = (listaBTC *)malloc(sizeof(listaBTC));
  if (novoEndereco == NULL)
  {
    printf("Alocacao de memoria falhou\n");
    return NULL;
  }
  novoEndereco->endereco = endereco;
  novoEndereco->next = NULL;
  return novoEndereco;
}

//-------------------------------------------------------------------------------

// Funcao para inserir um endereco na lista dos que tem BTC
void insereNoInicio(listaBTC **inicio, unsigned int endereco, int *contador)
{
  listaBTC *novoEndereco = criaNo(endereco);
  if (*inicio == NULL)
  {
    *inicio = novoEndereco;
    (*contador)++;
  }
  else
  {
    novoEndereco->next = *inicio;
    *inicio = novoEndereco;
    (*contador)++;
  }
}

//-------------------------------------------------------------------------------

// essa funcao serve para verificar se o minerador pode entrar na lista
// pois nao eh interessante a repeticao de enderecos na lista
int verificaSeMineradorPodeEntrarNaLista(listaBTC *enderecosComBTC, unsigned int endereco)
{
  listaBTC *tmp = enderecosComBTC;
  while (tmp != NULL)
  {
    if (tmp->endereco == endereco)
    {
      return 0;
    }
    tmp = tmp->next;
  }
  return 1;
}

//-------------------------------------------------------------------------------

// Funcao para printar a lista
void mostraLista(listaBTC *inicio)
{
  listaBTC *aux = inicio;
  int i=1;
  while (aux != NULL)
  {
    printf("(%d) %u - ", i, aux->endereco);
    aux = aux->next;
    i++;
  }
  printf("\n");
}

//-------------------------------------------------------------------------------

// printar hash
void printHash(unsigned char hash[], int length)
{
  int i;

  for (i = 0; i < length; ++i)
    printf("%02x", hash[i]);

  printf("\n");
}

//-------------------------------------------------------------------------------

void printarHashArquivo(FILE *pArq, unsigned char *hash)
{
  const char *FORMATO_BLOCO_MINERADO_HASH = "%02x";
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    fprintf(pArq, FORMATO_BLOCO_MINERADO_HASH, hash[i]);
  }
}

//-------------------------------------------------------------------------------
//---------------------------------FUNCOES ABP--------------------------------------------
/*
void criaNo(TNoABP** novoNo, BlocoMinerado dadosBloco) {
    *novoNo = (TNoABP*)malloc(sizeof(TNoABP));
    if(*novoNo == NULL){
      printf("\nNao foi possivel alocar memoria para ABP\n");
      return;
    }
    (*novoNo)->dadosBloco = dadosBloco;
    (*novoNo)->esq = NULL;
    (*novoNo)->dir = NULL;
}
*/
//-------------------------------------------------------------------------------
/*
void inserirArvore(TNoABP** raiz, BlocoMinerado dadosBloco) {
    TNoABP* newNode = criaNo(dadosBloco);

    if (*raiz == NULL) {
        *raiz = newNode;
        return;
    }

    TNoABP* atual = *raiz;
    TNoABP* pai = NULL;

    while (atual != NULL) {
        pai = atual;

        if (dadosBloco.bloco.nonce < atual->dadosBloco.bloco.nonce)
            atual = atual->esq;
        else if (dadosBloco.bloco.nonce > atual->dadosBloco.bloco.nonce)
            atual = atual->dir;
        else {
            // Handle duplicates if necessary
            // Here, we are assuming duplicates are allowed and inserting to the esq
            atual = atual->esq;
        }
    }

    if (dadosBloco.bloco.nonce < pai->dadosBloco.bloco.nonce)
        pai->esq = newNode;
    else
        pai->dir = newNode;
}
*/
//---------------------------------QUICK SORT-----------------------------------------------------


void troca(BlocoMinerado *a, BlocoMinerado *b){
  BlocoMinerado tmp = *a;
  *a = *b;
  *b = tmp;
}

int partition(BlocoMinerado *vetorBlocosMinerados, int menor, int maior){
  int pivot = vetorBlocosMinerados[maior].bloco.nonce;
  int i = menor -1;

  for(int j=menor; j<=maior; j++){
    if(vetorBlocosMinerados[j].bloco.nonce < pivot){
      i++;
      troca(&vetorBlocosMinerados[i], &vetorBlocosMinerados[j]);
    }
  }

  troca(&vetorBlocosMinerados[i+1], &vetorBlocosMinerados[maior]);
  return(i+1);
}

void quickSort(BlocoMinerado *vetorBlocosMinerados, int menor, int maior){
  if(menor < maior){
    int pi = partition(vetorBlocosMinerados, menor, maior);
    quickSort(vetorBlocosMinerados, menor, pi-1);
    quickSort(vetorBlocosMinerados, pi + 1, maior);
  }
}


//-------------------------------------------------------------------------------
/*
void insereNoFinalDaLista(listaMineradoresRepetidosABP **inicio, TIndiceMinerador blocoParaInserir){
     listaMineradoresRepetidosABP* novoNo = malloc(sizeof(listaMineradoresRepetidosABP));
     novoNo->dadosMinerador.minerador = blocoParaInserir.minerador;
     novoNo->dadosMinerador.blocoDoMinerador = blocoParaInserir.blocoDoMinerador;
     novoNo->next = NULL;
     if(*inicio = NULL){

     }
     listaMineradoresRepetidosABP *aux = *inicio;
     while(aux->next != NULL){
      aux = aux->next;
     }

     aux->next = novoNo;
}
*/
//-------------------------------------------------------------------------------
/*
                                              //not quite sure
void insereNoIterativo(TNoABPMinerador **raiz, TIndiceMinerador blocoParaInserir, listaMineradoresRepetidosABP**inicio){
    TNoABPMinerador *aux = *raiz;
    while(aux){
        if(blocoParaInserir.minerador < aux->dadosMinerador.minerador){
            raiz = &aux->esq;
        }
        else if(blocoParaInserir.minerador == aux->dadosMinerador.minerador){
          insereNoFinalDaLista(,blocoParaInserir)
        }
        else{
            raiz = &aux->dir;
        }
        aux = *raiz;
    }
    aux = malloc(sizeof(TNoABPMinerador));
    aux->dadosMinerador.minerador = blocoParaInserir.minerador;
    aux->dadosMinerador.blocoDoMinerador = blocoParaInserir.blocoDoMinerador;
    aux->esq = NULL;
    aux->dir = NULL;
    aux->listaRepetidos = NULL;
    *raiz = aux;
}
*/
//-------------------------------------------------------------------------------

/*
void printEmOrdem(TNoABP* root) {
    if (root != NULL) {
        printEmOrdem(root->esq);
        printf("Nonce: %d\nNumber: %d\n", root->dadosBloco.bloco.nonce, root->dadosBloco.bloco.numero);
        printEmOrdem(root->dir);
    }
}
*/

//-------------------------------------------------------------------------------


// funcao para preencher o bloco genesis com os campos
// solicitados no enunciado do projeto
void preencheBlocoGenesis(BlocoNaoMinerado *blocoGenesis)
{

  // PREENCHENDO VETOR DATA COM ZEROS
  for (int i = 0; i < 184; i++)
  {
    blocoGenesis->data[i] = 0;
  }

  // PREENCHENDO VETOR DE HASH ANTERIOR COM ZEROS, POIS ESSE É O PRIMEIRO BLOCO
  for (int i = 0; i < (SHA256_DIGEST_LENGTH); i++)
  {
    blocoGenesis->hashAnterior[i] = 0;
  }

  // COLOCANDO A FRASE DO BLOCO GÊNESIS
  char *str = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
  strcpy(blocoGenesis->data, str);

  // DEFININDO NONCE COMO 0 E NUMERO DO BLOCO COMO 1, COMO DITO NO ENUNCIADO
  blocoGenesis->nonce = 0;
  blocoGenesis->numero = 1;
}

//-------------------------------------------------------------------------------

// inicializar a carteira do projeto com zeros
void inicializaCarteira(unsigned int *carteira)
{
  for (int i = 0; i < 256; i++)
  {
    carteira[i] = 0;
  }
}

//-------------------------------------------------------------------------------
// funcao para minerar bloco genesis
BlocoMinerado minerarBlocoGenesis(BlocoNaoMinerado *blocoGenesis, unsigned int *carteira, listaBTC **enderecosComBTC, MTRand *r, int *contador)
{

  // vetor que armazenará o resultado do hash. Tamanho definidio pela libssl
  unsigned char hash[SHA256_DIGEST_LENGTH];

  // PROCESSO PARA DEFINIR NUMERO ALETORIO QUE REPRESENTA O MINERADOR DO BLOCO:
  unsigned char minerador = genRandLong(r) % 256;
  blocoGenesis->data[183] = minerador;

  // gerando um 1° hash para o bloco
  SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);

  // gerando um novo hash e atualizando o nonce
  // ate que hajam 2 zeros hexadecimais no comeco
  // esse eh o criterio para validar um hash
  // assim sabemos se um bloco foi minerado
  while (hash[0] != 0)
  {
    blocoGenesis->nonce++;
    SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);
  }

  // Agora que o bloco foi minerado
  // Vamos passar ele para a struct de bloco minerado
  BlocoMinerado blocoGenesisMinerado;

  // os campos do bloco nao minerado sao enviados para o bloco minerado
  blocoGenesisMinerado.bloco = *blocoGenesis;

  // passando o hash que valida o bloco para a struct de bloco minerado
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    blocoGenesisMinerado.hash[i] = hash[i];
  }

  // ATUALIZA CARTEIRA DO SISTEMA DEPOIS DA VALIDAÇÃO DO BLOCO
  carteira[minerador] = 50;

  // COLOCA PRIMEIRO MINERADOR NA LISTA DE ENDERECOS COM BTC
  insereNoInicio(enderecosComBTC, minerador, contador);

  return blocoGenesisMinerado;
}

//-------------------------------------------------------------------------------
// funcao para procurar um endereco de destino na lista de quem tem BTC
unsigned char procuraEndereco(listaBTC *enderecosComBTC, int indice)
{
  listaBTC *tmp = enderecosComBTC;
  for (int i = 0; i < indice; i++)
  {
    tmp = tmp->next;
  }
  return tmp->endereco;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// funcao para remover um endereco
// usada caso algum endereco que tinha BTC tenha transferido todas que tinha
void removerEndereco(listaBTC **enderecosComBTC, unsigned char endereco, int *contador)
{

  listaBTC *tmp = *enderecosComBTC;
  listaBTC *anterior = NULL;

  if (tmp != NULL && tmp->endereco == endereco)
  {
    *enderecosComBTC = tmp->next;
    // free(tmp);
    (*contador)--;
    return;
  }

  while (tmp != NULL && tmp->endereco != endereco)
  {
    anterior = tmp;
    tmp = tmp->next;
  }

  if (tmp != NULL)
  {
    anterior->next = tmp->next;
    // free(tmp);
    (*contador)--;
  }

  return;
}

//-------------------------------------------------------------------------------

void inicializarBloco(unsigned char *hashAnterior, BlocoNaoMinerado *blocoN, int numeroBloco, MTRand *r, int *contador, listaBTC *enderecosComBTC, unsigned int *carteira, unsigned char qtdTransacoes)
{
  // preenchendo o campo do numero que representa
  blocoN->numero = numeroBloco;

  // inicializando campo nonce
  blocoN->nonce = 0;

  // PREENCHENDO VETOR DATA COM ZEROS
  for (int i = 0; i < 184; i++)
  {
    blocoN->data[i] = 0;
  }

  // preenchendo o campo de hash anterior
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    blocoN->hashAnterior[i] = hashAnterior[i];
  }

  // printf("Quantidade de transacoes no bloco: %u\n", qtdTransacoes);
  //  definindo os elementos das transacoes
  if (numeroBloco == 2)
  {
    for (int i = 0; i < qtdTransacoes; i++)
    {
      for (int j = 0; j < 3; j++)
      {

        blocoN->data[(i * 3) + j] = 140;
        j++;

        // gerar endereco de destino
        blocoN->data[(i * 3) + j] = genRandLong(r) % 256;
        j++;

        // definir quantidade de bitcoins
        unsigned char qtdBTC = (genRandLong(r) % (carteira[140] + 1));
        blocoN->data[(i * 3) + j] = qtdBTC;

        // atualizando carteira para nao haver inconsistencia
        // por exemplo um endereco que possui 10BTC estar repassar 20BTC
        // printf("quantidade na carteira[140]: %u\n", carteira[140]);
        carteira[140] = carteira[140] - qtdBTC;
      }
    }
  }
  else
  {
    for (int i = 0; i < qtdTransacoes; i++)
    {
      for (int j = 0; j < 3; j++)
      {

        // gerar endereco de origem
        int indice = genRandLong(r) % (*contador);
        unsigned char endOrigem = procuraEndereco(enderecosComBTC, indice);
        blocoN->data[(i * 3) + j] = endOrigem;
        j++;

        // gerar endereco de destino
        blocoN->data[(i * 3) + j] = genRandLong(r) % 256;
        j++;

        // definir quantidade de bitcoins
        unsigned char qtdBTC = genRandLong(r) % (carteira[endOrigem] + 1);
        blocoN->data[(i * 3) + j] = qtdBTC;

        // atualizando carteira para nao haver inconsistencia
        // por exemplo um endereco que possui 10BTC estar repassar 20BTC
        carteira[endOrigem] = carteira[endOrigem] - qtdBTC;
      }
    }
  }

  // decidindo quem será o minerador do bloco
  unsigned char minerador = genRandLong(r) % 256;
  blocoN->data[183] = minerador;
}

//-------------------------------------------------------------------------------

BlocoMinerado minerarBloco(BlocoNaoMinerado *blocoN, unsigned int *carteira, listaBTC **enderecosComBTC, int *contador, unsigned char qtdTransacoes)
{

  BlocoMinerado blocoNMinerado;

  unsigned char hash[SHA256_DIGEST_LENGTH];

  // gerando um hash para o bloco
  SHA256((unsigned char *)blocoN, sizeof(BlocoNaoMinerado), hash);

  // alterando hash e nonce ate
  // o hash ser valido
  while (hash[0] != 0)
  {
    blocoN->nonce++;
    SHA256((unsigned char *)blocoN, sizeof(BlocoNaoMinerado), hash);
  }

  // preenchendo campo hash do bloco minerado com o hash valido
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    blocoNMinerado.hash[i] = hash[i];
  }

  // atribuindo as 50 BTC para o minerador do bloco
  unsigned char minerador = blocoN->data[183];
  carteira[minerador] += 50;

  // inserindo minerador na lista dos que tem BTC
  // caso ele nao esteja
  if (verificaSeMineradorPodeEntrarNaLista(*enderecosComBTC, minerador))
  {
    insereNoInicio(enderecosComBTC, minerador, contador);
  }

  // repassando campos do blocoN para o bloco minerado
  blocoNMinerado.bloco = *blocoN;

  // atualizando a carteira apos validada as transacoes
  for (int i = 0; i < qtdTransacoes; i++)
  {
    for (int j = 0; j < 3; j++)
    {

      // agora que o bloco foi validado
      // devemos atualizar os saldos
      // dos enderecos que receberam BTC
      if (j == 1)
      {
        // printf("indice: %u\n", blocoN->data[(i*3)+1]);
        carteira[blocoN->data[(i * 3) + 1]] += blocoN->data[(i * 3) + 2];

        if (blocoN->data[(i * 3) + 2] != 0)
        {
          if (verificaSeMineradorPodeEntrarNaLista(*enderecosComBTC, blocoN->data[(i * 3) + 1]))
          {
            insereNoInicio(enderecosComBTC, blocoN->data[(i * 3) + 1], contador);
          }
        }
      }
    }
  }

  // antes de retornar o bloco minerado
  // vamos verificar a necessidade de remover
  // algum endereco da lista de enderecos com BTC
  // afinal, alguem pode ter doado todas as bitcoins que tinha num bloco
  listaBTC *tmp = *enderecosComBTC;
  while (tmp != NULL)
  {
    if (carteira[tmp->endereco] == 0)
    {
      removerEndereco(enderecosComBTC, tmp->endereco, contador);
    }
    tmp = tmp->next;
  }

  return blocoNMinerado;
}
//---------------------------------------------------------------------------------------
  void printarDadosDeUmBloco(BlocoMinerado blocoMinerado){
    printf("\n");
      printf("=======================================\n");
      printf("Bloco: %d\n", blocoMinerado.bloco.numero);
      printf("Nonce: %d\n", blocoMinerado.bloco.nonce);
      printf("Minerador: %u\n", blocoMinerado.bloco.data[183]);
      printf("Hash anterior: ");
      printHash(blocoMinerado.bloco.hashAnterior, SHA256_DIGEST_LENGTH);
      printf("Hash valido: ");
      printHash(blocoMinerado.hash, SHA256_DIGEST_LENGTH);

      printf("\nTRANSACOES\n");
      for (int i = 0; i < 184; i++)
      {
        printf("[%u] - ", blocoMinerado.bloco.data[i]);
        if ((i + 1) % 3 == 0)
        {
          printf("\n");
        }
      }
      printf("\n=======================================\n");
  }
//---------------------------------------------------------------------------------------
int binarySearchBaseadoNoNumero(BlocoMinerado *vetorBlocosMinerados, int menor, int maior, unsigned int numeroBlocoProcurado)
{
  while (menor <= maior)
  {

    int meio = menor + (maior - menor) / 2;

    if (vetorBlocosMinerados[meio].bloco.numero == numeroBlocoProcurado)
    {
      printarDadosDeUmBloco(vetorBlocosMinerados[meio]);
      return 1;
    }
    else if (vetorBlocosMinerados[meio].bloco.numero < numeroBlocoProcurado)
    {
      menor = meio + 1;
    }
    else
    {
      maior = meio - 1;
    }
  }
  return -1;
}
//---------------------------------------------------------------------------------------------------------------

int binarySearchBaseadoNonce(BlocoMinerado* arr, int menor,int maior, unsigned int target) {
    int found = 0;

    while (menor <= maior) {
        int mid = menor + (maior - menor) / 2;

        if (arr[mid].bloco.nonce == target) {
            found = 1;
            int left = mid - 1;
            int right = mid + 1;
            ;
            // Print all occurrences of target towards left
            while (left >= 0 && arr[left].bloco.nonce == target) {
                printarDadosDeUmBloco(arr[left]);
                left--;
            }

            printarDadosDeUmBloco(arr[mid]);  // Print the current occurrence

            // Print all occurrences of target towards right
            while (right < (maior+1) && arr[right].bloco.nonce == target) {

                printarDadosDeUmBloco(arr[right]);
                right++;
            }

            return 1;
            break;
        }

        if (arr[mid].bloco.nonce < target)
            menor = mid + 1;
        else
            maior = mid - 1;
    }

}

//----------------------------------------- FUNÇÕES MANIPULAÇÃO DE ARQUIVOS -------------------------------------
void criarArquivoIndices(listaBTC *inicio, const char *nomeArquivo)
{
  FILE *arquivo = fopen(nomeArquivo, "w");
  if (arquivo == NULL)
  {
    printf("Erro ao criar o arquivo de índices.\n");
    return;
  }

  listaBTC *aux = inicio;
  unsigned int posicao = 0;
  while (aux != NULL)
  {
    fprintf(arquivo, "%u %u\n", aux->endereco, posicao);
    aux = aux->next;
    posicao++;
  }

  fclose(arquivo);
}

//--------------------------------------------------------------------------------------

void escreverArquivoBinarioDeIndicesBaseadoNoNonce(BlocoMinerado *vetorBlocosMinerados, int quantidadeDeElementosParaEscrever, const char* nomeArquivo)
{
  FILE *arqIndiceNonce = fopen(nomeArquivo, "ab");
  if(arqIndiceNonce == NULL){
    printf("Erro ao tentar abrir arquivo de indices baseados no Nonce");
  }
  
  // Escreve os indices no arquivo
    for (int i = 0; i < 16; i++) {
        fwrite(&i, sizeof(unsigned int), 1, arqIndiceNonce);
    }

    // Escreve os nonces no arquivo
    for (int i = 0; i < 16; i++) {
        fwrite(&(vetorBlocosMinerados[i].bloco.nonce), sizeof(unsigned int), 1, arqIndiceNonce);
    }

  fclose(arqIndiceNonce);
}
//---------------------------------------------------------------------------------------------------
void escreverArquivoBinarioDeIndicesBaseadoNoMinerador(BlocoMinerado *vetorBlocosMinerados, int quantidadeDeElementosParaEscrever, const char* nomeArquivo){
    FILE *arqBinario = fopen(nomeArquivo, "ab");
    if (arqBinario == NULL)
    {
      printf("\nErro ao criar o arquivo binario de indices sobre o minerador.\n");
      return;
    }

    /*
    unsigned char elementosParaEscrever[16];
    for (int i = 0; i < 16; i++) {
        elementosParaEscrever[i] = vetorBlocosMinerados[i].bloco.data[183];
    }
    */

    TIndiceMinerador vetorIndicesMineradores[16];
    for(int i=0; i<16; i++){
      vetorIndicesMineradores[i].minerador = vetorBlocosMinerados[i].bloco.data[183];
      vetorIndicesMineradores[i].blocoDoMinerador = vetorBlocosMinerados[i];
    }

    size_t quantidadeDeElementosEscritos = fwrite(vetorIndicesMineradores, sizeof(TIndiceMinerador), quantidadeDeElementosParaEscrever, arqBinario);

    if (quantidadeDeElementosEscritos != quantidadeDeElementosParaEscrever)
    {
      printf("Erro na escrita do arquivo binario");
    }

  fclose(arqBinario);
}
//---------------------------------------------------------------------------------------------------
void lerArquivoBinarioDeIndices(const char* nomeArquivo)
{
    // Abre o arquivo binário para leitura
    FILE* arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    unsigned int indice;
    unsigned int nonce;

    // Lê os índices e nonces do arquivo e exibe na tela
    while (fread(&indice, sizeof(unsigned int), 1, arquivo) == 1 && fread(&nonce, sizeof(unsigned int), 1, arquivo) == 1) {
        printf("Índice: %u, Nonce: %u\n", indice, nonce);
    }

    // Fecha o arquivo
    fclose(arquivo);
}
//-------------------------------------------------------------------------------
void imprimirIndices(const char* nomeArquivo) {
    FILE* arqIndiceNonce = fopen(nomeArquivo, "rb");
    if (arqIndiceNonce == NULL) {
        printf("Erro ao tentar abrir o arquivo de índices baseados no Nonce");
        return;
    }

    unsigned int nonce;
    unsigned int index;
    size_t elementosLidos;

    while ((elementosLidos = fread(&nonce, sizeof(unsigned int), 1, arqIndiceNonce)) == 1) {
        elementosLidos += fread(&index, sizeof(unsigned int), 1, arqIndiceNonce);

        printf("Nonce: %u, Index: %u\n", nonce, index);

        if (elementosLidos != 2) {
            printf("Erro ao ler o arquivo de índices baseados no Nonce");
            fclose(arqIndiceNonce);
            return;
        }
    }

    fclose(arqIndiceNonce);
}


//-------------------------------------------------------------------------------
void escreverArquivoBinario(BlocoMinerado *vetorBlocosMinerados, int quantidadeDeElementosParaEscrever, const char *nomeArquivo)
{
  FILE *arqBinario = fopen(nomeArquivo, "ab");
  if (arqBinario == NULL)
  {
    printf("Erro ao criar o arquivo binario de blocos minerados.\n");
    return;
  }

  size_t quantidadeDeElementosEscritos = fwrite(vetorBlocosMinerados, sizeof(BlocoMinerado), quantidadeDeElementosParaEscrever, arqBinario);
  

  if (quantidadeDeElementosEscritos != quantidadeDeElementosParaEscrever)
  {
    printf("Erro na escrita do arquivo binario");
  }

  fclose(arqBinario);
}


//-------------------------------------------------------------------------------

void escreverArquivoTexto(BlocoMinerado *vetorBlocosMinerados)
{
  const char *FORMATO_BLOCO_MINERADO = "\nBloco: %d\nNonce: %d\nMinerador: %u\n";
  const char *FORMATO_BLOCO_MINERADO_DATA = "[%u] -";
  FILE *pArqTexto = fopen("blocos_minerados.txt", "a+");
  if (pArqTexto == NULL)
  {
    printf("Erro ao criar/abrir arquivo texto de blocos minerados.\n");
    return;
  }
  for (int i = 0; i < 16; i++)
  {
    fprintf(pArqTexto, FORMATO_BLOCO_MINERADO, vetorBlocosMinerados[i].bloco.numero, vetorBlocosMinerados[i].bloco.nonce, vetorBlocosMinerados[i].bloco.data[183]);
    fprintf(pArqTexto, "Hash anterior: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].bloco.hashAnterior);
    fprintf(pArqTexto, "\n");
    fprintf(pArqTexto, "Hash valido: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].hash);
    fprintf(pArqTexto, "\n");
    for (unsigned char j = 0; j < 184; j++)
    {
      fprintf(pArqTexto, FORMATO_BLOCO_MINERADO_DATA, vetorBlocosMinerados[i].bloco.data[j]);

      if ((j + 1) % 3 == 0)
      {
        fprintf(pArqTexto, "\n");
      }
    }
    fprintf(pArqTexto, "\n=======================================");
  }
  fclose(pArqTexto);
}

//--------------------------------------------------------------------------------------

void escreverArquivoTextoComGenesis(BlocoMinerado *vetorBlocosMinerados)
{
  const char *FORMATO_BLOCO_MINERADO = "Bloco: %d\nNonce: %d\nMinerador: %u\n";
  const char *FORMATO_BLOCO_MINERADO_DATA = "[%u] -";
  const char *FORMATO_BLOCO_MINERADO_DATA_GENESIS = "%c";
  FILE *pArqTexto = fopen("blocos_minerados.txt", "a+");
  if (pArqTexto == NULL)
  {
    printf("Erro ao criar/abrir arquivo texto de blocos minerados.\n");
    return;
  }
  for (int i = 0; i < 16; i++)
  {
    fprintf(pArqTexto, FORMATO_BLOCO_MINERADO, vetorBlocosMinerados[i].bloco.numero, vetorBlocosMinerados[i].bloco.nonce, vetorBlocosMinerados[i].bloco.data[183]);
    fprintf(pArqTexto, "Hash anterior: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].bloco.hashAnterior);
    fprintf(pArqTexto, "\n");
    fprintf(pArqTexto, "Hash valido: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].hash);
    fprintf(pArqTexto, "\n");
    for (unsigned char j = 0; j < 184; j++)
    {
      if (i == 0)
      {
        fprintf(pArqTexto, FORMATO_BLOCO_MINERADO_DATA_GENESIS, vetorBlocosMinerados[i].bloco.data[j]);
      }
      else
      {
        fprintf(pArqTexto, FORMATO_BLOCO_MINERADO_DATA, vetorBlocosMinerados[i].bloco.data[j]);
        if ((j + 1) % 3 == 0)
        {
          fprintf(pArqTexto, "\n");
        }
      }
    }
    fprintf(pArqTexto, "\n=======================================\n");
  }
  fclose(pArqTexto);
}

//-------------------------------------------------------------------------------------------------------------------
void imprimirDadosArquivo(const char* nomeArquivo)
{
  FILE* arquivo = fopen(nomeArquivo, "rb");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    return;
  }

  BlocoMinerado bloco;
  //unsigned char minerador;
  for (int i = 0; i < 16; i++) {
    fread(&bloco, sizeof(BlocoMinerado), 1, arquivo);

    printf("Bloco %d:\n", i + 1);
    printf("Numero: %u\n", bloco.bloco.numero);
    printf("Nonce: %u\n", bloco.bloco.nonce);
    //printf("Data: %u\n", minerador);
    printf("Hash Anterior: ");
    for (int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
      printf("%02x", bloco.bloco.hashAnterior[j]);
    }
    printf("\n");
    printf("Hash valido: ");
    for (int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
      printf("%02x", bloco.hash[j]);
    }
    printf("\n\n");
  }

  fclose(arquivo);
}
//-------------------------------------------------------------------------------------------------------------------
  void imprimirDadosArquivoTeste(const char* nomeArquivo)
{
  FILE* arquivo = fopen(nomeArquivo, "rb");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    return;
  }

  TIndiceMinerador index; 
  //unsigned char minerador;
  for (int i = 0; i < 16; i++) {
    fread(&index, sizeof(TIndiceMinerador), 1, arquivo);
    printf("Minerador: %u\n", index.minerador);
    printf("Nonce: %u\n", index.blocoDoMinerador.bloco.nonce);
    //printf("Deslocamento: %u\n", index.deslocamento);
  }

  fclose(arquivo);
}
//-------------------------------------------------------------------------------------------------------------------
void procurarBlocoEmArquivoBinario(unsigned char *nomeDoArquivo, int quantidadeDeElementosEscritosNoArquivo, unsigned int numeroBlocoProcurado)
{
  FILE *arqBinario = fopen(nomeDoArquivo, "rb");
  if (arqBinario == NULL)
  {
    printf("Nao foi possivel abrir o arquivo binario para leitura\nNo contexto de realizar uma busca\n");
    return;
  }
  BlocoMinerado vetorBlocosMinerados[16];

  // calculo para saber a quantidade de "chunks" escritas no arquivo
  int quantidadeDeChunksEscritos = quantidadeDeElementosEscritosNoArquivo / 16;

  // se nao for multiplo de 16 haverao elementos restantes
  int elementosRestantes = quantidadeDeElementosEscritosNoArquivo % 16;

  for (int chunk = 0; chunk < quantidadeDeChunksEscritos; chunk++)
  {
    fread(vetorBlocosMinerados, sizeof(BlocoMinerado), 16, arqBinario);
    int flag = binarySearchBaseadoNoNumero(vetorBlocosMinerados, 0, 15, numeroBlocoProcurado);
    if(flag==1){
      fclose(arqBinario);
      return;
    }
  }

  fclose(arqBinario);
}
//-------------------------------------------------------------------------------------------------------------------
void procurarNoArquivoDeIndicesMinerador(unsigned int enderecoMinerador, int quantidadeDeBlocos, unsigned char *nomeDoArquivo, int quantidadeDeElementosEscritosNoArquivo)
{
  FILE *arqBinario = fopen(nomeDoArquivo, "rb");
  if (arqBinario == NULL)
  {
    printf("Nao foi possivel abrir o arquivo binario para leitura\nNo contexto de realizar uma busca no arquivo de indices minerador\n");
    return;
  }
  TIndiceMinerador vetorBlocosMineradores[16];

  int quantidadeDeChunksEscritos = quantidadeDeElementosEscritosNoArquivo / 16;
  int contadorQtdBlocos=0;
  for (int chunk = 0; chunk < quantidadeDeChunksEscritos; chunk++)
  {
    fread(vetorBlocosMineradores, sizeof(TIndiceMinerador), 16, arqBinario);
    //int flag = binarySearchBaseadoNoNumero(vetorBlocosMinerados, 0, 15, numeroBlocoProcurado);
    for(int i=0; i<16; i++){
      if(vetorBlocosMineradores[i].minerador == enderecoMinerador){
        printarDadosDeUmBloco(vetorBlocosMineradores[i].blocoDoMinerador);
        contadorQtdBlocos++;
      }
    }
    if(contadorQtdBlocos==quantidadeDeBlocos){
      fclose(arqBinario);
      return;
    }
  }
  printf("Esse endereco minerou menos que %d bloco(s)\nMas esses foram os blocos que minerou\n", quantidadeDeBlocos);

}
//-------------------------------------------------------------------------------------------------------------------
void procurarQuemMinerouMaisBlocos(const char *nomeDoArquivo, int quantidadeDeElementosEscritosNoArquivo)
{
  FILE *arqBinario = fopen(nomeDoArquivo, "rb");
  if (arqBinario == NULL)
  {
    printf("Nao foi possivel abrir o arquivo binario para leitura\nNo contexto de realizar uma busca no arquivo de indices minerador\n");
    return;
  }
  TIndiceMinerador vetorBlocosMineradores[16];
  unsigned int contagemDeMineracoesPorEndereco[255];

  inicializaCarteira(contagemDeMineracoesPorEndereco);

  int quantidadeDeChunksEscritos = quantidadeDeElementosEscritosNoArquivo / 16;

  unsigned int maiorQtdMineracoes=0;
  for (int chunk = 0; chunk < quantidadeDeChunksEscritos; chunk++)
  {
    fread(vetorBlocosMineradores, sizeof(TIndiceMinerador), 16, arqBinario);
    // int flag = binarySearchBaseadoNoNumero(vetorBlocosMinerados, 0, 15, numeroBlocoProcurado);
    for (int i = 0; i < 16; i++)
    {
      contagemDeMineracoesPorEndereco[vetorBlocosMineradores[i].minerador]++;
      if(contagemDeMineracoesPorEndereco[vetorBlocosMineradores[i].minerador] > maiorQtdMineracoes){
        maiorQtdMineracoes = contagemDeMineracoesPorEndereco[vetorBlocosMineradores[i].minerador];
      }
    }
    
  }
  for(int i=0; i<256; i++){
    if(contagemDeMineracoesPorEndereco[i] == maiorQtdMineracoes){
      printf("Endereco %u minerou a maior quantidade de blocos(%u)\n", i, maiorQtdMineracoes);
    }
  }
  fclose(arqBinario);
  return;
}
//-------------------------------------------------------------------------------------------------------------------
void procurarNonceEmArquivoBinario(unsigned char *nomeDoArquivo, int quantidadeDeElementosEscritosNoArquivo, unsigned int nonceProcurado)
{
  FILE *arqBinario = fopen(nomeDoArquivo, "rb");
  if (arqBinario == NULL)
  {
    printf("Nao foi possivel abrir o arquivo binario para leitura\nNo contexto de realizar uma busca\n");
    return;
  }
  BlocoMinerado vetorBlocosMinerados[16];

  // calculo para saber a quantidade de "chunks" escritas no arquivo
  int quantidadeDeChunksEscritos = quantidadeDeElementosEscritosNoArquivo / 16;

  // se nao for multiplo de 16 haverao elementos restantes
  int elementosRestantes = quantidadeDeElementosEscritosNoArquivo % 16;

  for (int chunk = 0; chunk < quantidadeDeChunksEscritos; chunk++)
  {
    fread(vetorBlocosMinerados, sizeof(BlocoMinerado), 16, arqBinario);
    int flag = binarySearchBaseadoNonce(vetorBlocosMinerados, 0, 15, nonceProcurado);
    if(flag==1){
      fclose(arqBinario);
      return;
    }
  }

  fclose(arqBinario);
}
//-------------------------------------------------------------------------------------------------------------------
/// Função para imprimir o endereço que minerou mais blocos
void imprimirEnderecoMaisMinerou(listaBTC *enderecosComBTC)
{
  // Array para contar o número de blocos minerados por endereço
  int contadorEnderecos[256] = {0};

  // Percorrer a lista de endereços e contar os blocos minerados por cada endereço
  listaBTC *tmp = enderecosComBTC;
  while (tmp != NULL)
  {
    contadorEnderecos[tmp->endereco]++;
    tmp = tmp->next;
  }

  // Encontrar o endereço que minerou mais blocos
  unsigned int enderecoMaisMinerou = 0;
  int maxBlocosMinerados = 0;
  int empate = 0;

  for (int i = 0; i < 256; i++)
  {
    if (contadorEnderecos[i] > maxBlocosMinerados)
    {
      enderecoMaisMinerou = i;
      maxBlocosMinerados = contadorEnderecos[i];
      empate = 0;
    }
    else if (contadorEnderecos[i] == maxBlocosMinerados)
    {
      empate = 1;
    }
  }

  // Imprimir o endereço que minerou mais blocos ou os endereços em caso de empate
  if (empate)
  {
    printf("\nHouve um empate na contagem de blocos minerados entre os seguintes endereços:\n");
    for (int i = 0; i < 256; i++)
    {
      if (contadorEnderecos[i] == maxBlocosMinerados)
      {
        printf("\nEndereço: %u ", i);
      }
    }
    printf("\n\n->>>Todos os endereços listados acima mineraram %d bloco(s)", maxBlocosMinerados);
  }
  else
  {
    printf("O endereço que minerou mais blocos é: %u - Quantidade de vezes minerado: %d\n", enderecoMaisMinerou, maxBlocosMinerados);
  }
}

//--------------------------------------------------------------------------------------------------------------------
// função para imprimir o conteúdo da carteira, mas é apenas teste, depois será excluída
void imprimirCarteira(unsigned int *carteira)
{
  for (int i = 0; i < 256; i++)
  {
    if (carteira[i] > 0)
    {
      printf("Endereço %d: %u BTC\n", i, carteira[i]);
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  MTRand r = seedRand(1234567);
  int contador = 0;
  //TNoABP *raizArvoreNonce = NULL;
  int quantidadeDeElementosEscritosNoArquivoBinario = 0;
  // instanciando e preenchendo bloco genesis
  // da maneira solicitada no enunciado
  BlocoNaoMinerado blocoGenesis;
  preencheBlocoGenesis(&blocoGenesis);

  // inicializando carteira conforme pedido no enunciado
  unsigned int carteira[256];
  inicializaCarteira(carteira);

  // printf("CARTEIRA[122]: %u\n", carteira[122]);

  // cria lista dos enderecos com BTC
  listaBTC *enderecosComBTC = NULL;

  // cria vetor de 16 posições para armazenar blocos minerados
  BlocoMinerado vetorBlocosMinerados[16];

  // minerando o bloco genesis
  BlocoMinerado blocoGenesisMinerado = minerarBlocoGenesis(&blocoGenesis, carteira, &enderecosComBTC, &r, &contador);

  vetorBlocosMinerados[0] = blocoGenesisMinerado;
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
  int qtdBlocosMinerar;
  printf("\nDigite a quantidade de blocos que deseja minerar: ");
  scanf("%d", &qtdBlocosMinerar);

  TBlocoETransacoes vetorBlocosComTransacoes[qtdBlocosMinerar];
  vetorBlocosComTransacoes[0].qtdTransacoes = 0;
  vetorBlocosComTransacoes[0].bloco = blocoGenesisMinerado; 
  // Minerar 30.000 blocos agora
  // i=29999
  for (int i = 0; i < (qtdBlocosMinerar-1); i++)
  {

    BlocoNaoMinerado blocoN;

    // decidindo quantidade de transacoes no bloco
    unsigned char qtdTransacoes = genRandLong(&r) % 62;

    if (i == 0)
    {
      inicializarBloco(blocoGenesisMinerado.hash, &blocoN, 2, &r, &contador, enderecosComBTC, carteira, qtdTransacoes);
    }
    else
    {

      inicializarBloco(hashAnterior, &blocoN, i + 2, &r, &contador, enderecosComBTC, carteira, qtdTransacoes);
    }

    BlocoMinerado blocoNMinerado = minerarBloco(&blocoN, carteira, &enderecosComBTC, &contador, qtdTransacoes);

    int indice = (i + 1) % 16;
    vetorBlocosMinerados[indice] = blocoNMinerado;

    vetorBlocosComTransacoes[i+1].qtdTransacoes = qtdTransacoes;
    vetorBlocosComTransacoes[i+1].bloco = blocoNMinerado;

    for (int k = 0; k < SHA256_DIGEST_LENGTH; k++)
    {
      hashAnterior[k] = blocoNMinerado.hash[k];
    }
    // printf("i: %d\n", i);

    // verificando se eh para escrever no arquivo
    if ((i + 1) % 15 == 0 && i == 14)
    {

      escreverArquivoTextoComGenesis(vetorBlocosMinerados);

      escreverArquivoBinario(vetorBlocosMinerados, 16, "blocos_minerados.bin");
      
      escreverArquivoBinarioDeIndicesBaseadoNoMinerador(vetorBlocosMinerados, 16, "arquivo_indices_minerador.bin");
      
      
      
      
      
      quantidadeDeElementosEscritosNoArquivoBinario += 16;

    }
    /*
    else if((blocoN.numero-constanteCorrecao)%15==0 && i!=15 && i!=0){
      //printf("\nvalor do i: %d\n", i);
      escreverArquivoTexto(vetorBlocosMinerados);
      constanteCorrecao++;
    }
    */
    else if (indice == 15)
    {
      escreverArquivoTexto(vetorBlocosMinerados);
      escreverArquivoBinario(vetorBlocosMinerados, 16, "blocos_minerados.bin");
      escreverArquivoBinarioDeIndicesBaseadoNoMinerador(vetorBlocosMinerados, 16, "arquivo_indices_minerador.bin");
      //quickSort(vetorBlocosMinerados, 0, 15);
      //escreverArquivoBinario(vetorBlocosMinerados, 16, "arquivo_indices_nonce.bin");
      quantidadeDeElementosEscritosNoArquivoBinario += 16;
    }

    /*
    printf("\nTRANSACOES\n");
    for (int i = 0; i < 184; i++)
    {
      printf("[%u] - ", blocoN.data[i]);
      if ((i + 1) % 3 == 0)
      {
        printf("\n");
      }
    }
    */
    //insereNoIterativo(&raizArvoreNonce, blocoNMinerado);
    /*
    printf("\n");
    printf("Bloco: %d\n", blocoN.numero);
    printf("Nonce: %d\n", blocoN.nonce);
    printf("Minerador: %u\n", blocoN.data[183]);
    printf("Hash anterior: ");
    printHash(blocoN.hashAnterior, SHA256_DIGEST_LENGTH);
    printf("Hash valido: ");
    printHash(blocoNMinerado.hash, SHA256_DIGEST_LENGTH);
    */
    // printf("lista de enderecos com BTC: ");
    //mostraLista(enderecosComBTC);
    // printf("\n================================\n");
    // printf("pode ? %d", verificaSeMineradorPodeEntrarNaLista(enderecosComBTC, 139));
  }
  /*
  printf("carteira[136]: %u\n", carteira[136]);
  printf("carteira[139]: %u\n", carteira[139]);
  printf("carteira[250]: %u\n", carteira[250]);
  printf("carteira[140]: %u\n", carteira[140]);
  */

  // unsigned char hashAnterior[SHA256_DIGEST_LENGTH];

  // minerarBloco();
  /*
  //vetor que armazenará o resultado do hash. Tamanho definidio pela libssl
  unsigned char hash[SHA256_DIGEST_LENGTH];

  //A funcao SHA256 requer tres parametros
  //1. o ponteiro para um vetor de unsigned char contendo o dado cujo hash você deseja calcular
  //2. o tamanho em bytes do dado cujo hash você deseja calcular
  //3. o ponteiro para um vetor de unsigned char que armazenará o resultado do hash calculado.
  SHA256((unsigned char *)&blocoGenesis, sizeof(BlocoNaoMinerado), hash);

  while(hash[0]!=0){
    blocoGenesis.nonce++;
    SHA256((unsigned char *)&blocoGenesis, sizeof(BlocoNaoMinerado), hash);
  }
  */

  // printf("Nonce: %d\n", blocoGenesisMinerado.bloco.nonce);
  // printf("Minerador: %u\n", blocoGenesisMinerado.bloco.data[183]);
  //  printf("aleatorio teste: %lu", genRandLong(&r)%61);
  //  printf("Hash: ");
  //  printHash(hash, SHA256_DIGEST_LENGTH);
  //  printf("Carteira[140]: %u", carteira[140]);
  //  printf("hash bloco genesis minerado: ");
  //  printHash(blocoGenesisMinerado.hash, SHA256_DIGEST_LENGTH);

  // mostraLista(enderecosComBTC);

  // MTRand r = seedRand(1234567);
  /*
   */
  /*
  for (int i = 0; i < 15; i++)
  {
    printf("[%u] - ", blocoN->data[i]);
    if ((i + 1) % 3 == 0)
    {
      printf("\n");
    }
  }
  */
  // mostraLista(enderecosComBTC);
  // insereNoInicio(&enderecosComBTC, (unsigned char)100, &contador);
  // insereNoInicio(&enderecosComBTC, (unsigned char)50, &contador);
  // insereNoInicio(&enderecosComBTC, (unsigned char)20, &contador);
  // insereNoInicio(&enderecosComBTC, (unsigned char)70, &contador);

  // mostraLista(enderecosComBTC);
  // printf("qtd elementos: %d", contador);

  // removerEndereco(&enderecosComBTC, 70, &contador);
  // removerEndereco(&enderecosComBTC, 140, &contador);

  // mostraLista(enderecosComBTC);

  //printf("\nqtd elementos: %d\n", contador);

  //------------------------ FUNÇÕES QUE CRIAM ARQUIVOS QUE SERÃO UTILIZADOS NOS CASES 6,7,8 e 9 -----------------------------

  // lerArquivoBinario("blocos_minerados.bin");
  //----------------------------------------------------------------------------------------------------------------------------

  int choice;
  do
  {
    printf("--------------------------- MENU ---------------------------------------");
    printf("\n0. Inicializar todo o programa\n");
    printf("1. Imprimir endereco(s) com mais bitcoins\n");
    printf("2. Imprimir endereco(s) que minerou mais blocos\n");
    printf("3. Imprimir bloco(s) com mais transacoes\n");
    printf("4. Imprimir bloco(s) com menos transacoes\n");
    printf("5. Imprimir quantidade media de bitcoins por bloco\n");
    printf("6. Imprimir todos os campos de um bloco dado seu numero\n");
    printf("7. Imprimir todos os campos dos N primeiros blocos minerados por um endereco\n");
    printf("8. Imprimir em ordem crescente de quantidade de transacoes todos os campos dos N primeiros blocos\n");
    printf("9. Imprimir todos os campos de todos os blocos que tem um dado nonce\n");
    // criando um case 10, só pra testar e verificar algumas coisas sobre a carteira
    printf("10. Imprimir carteira\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 0:
    {
    }
    break;
    case 1:
    {
      unsigned int maiorQtdBTC=0;
      unsigned int enderecoComMaisBTC;

      //loop na carteira para saber a maior quantidade de BTC
      for(int i=0; i<256; i++){
        if(carteira[i]>maiorQtdBTC){
          maiorQtdBTC = carteira[i];
        }
      }

      //loop na carteira imprimindo todos os enderecos que possuem a maior quantia de BTC
      for(int i=0; i<256; i++){
        if(carteira[i]==maiorQtdBTC){
          printf("\n=======ENDERECO(s) COM MAIS BTC=======\n");
          printf("Endereco %u tem %uBTC\n", i, maiorQtdBTC);
        }
      }
    }
    break;
    case 2:
    {
      /*
      printf("\n");
      imprimirEnderecoMaisMinerou(enderecosComBTC);
      printf("\n");
      */
      procurarQuemMinerouMaisBlocos("arquivo_indices_minerador.bin", quantidadeDeElementosEscritosNoArquivoBinario);   
    }
    break;
    case 3:
    {
      unsigned int maiorQtdTransacoes=0;
      for(int i=0; i<qtdBlocosMinerar; i++){
        if(vetorBlocosComTransacoes[i].qtdTransacoes > maiorQtdTransacoes){
          maiorQtdTransacoes = vetorBlocosComTransacoes[i].qtdTransacoes;
        }
      }
      printf("Maior quantida de transacoes: %u", maiorQtdTransacoes);
      printf("\n============HASH DESSE(s) BLOCO(s)===============\n");
      for(int i=0; i<qtdBlocosMinerar; i++){
        if(vetorBlocosComTransacoes[i].qtdTransacoes == maiorQtdTransacoes){
          printf("Hash do bloco numero %u: ", vetorBlocosComTransacoes[i].bloco.bloco.numero);
          printHash(vetorBlocosComTransacoes[i].bloco.hash, SHA256_DIGEST_LENGTH);

          //printarDadosDeUmBloco(vetorBlocosComTransacoes[i].bloco);
        }
      }
    }
    break;
    case 4:
    {
      unsigned int menorQtdTransacoes=61;
      for(int i=0; i<qtdBlocosMinerar; i++){
        if(vetorBlocosComTransacoes[i].qtdTransacoes < menorQtdTransacoes){
          menorQtdTransacoes = vetorBlocosComTransacoes[i].qtdTransacoes;
        }
      }
      printf("Menor quantida de transacoes: %u", menorQtdTransacoes);
      printf("\n============HASH DESSE(s) BLOCO(s)===============\n");
      for(int i=0; i<qtdBlocosMinerar; i++){
        if(vetorBlocosComTransacoes[i].qtdTransacoes == menorQtdTransacoes){
          printf("Hash do bloco numero %u: ", vetorBlocosComTransacoes[i].bloco.bloco.numero);
          printHash(vetorBlocosComTransacoes[i].bloco.hash, SHA256_DIGEST_LENGTH);

          //printarDadosDeUmBloco(vetorBlocosComTransacoes[i].bloco);
        }
      }
    }
    break;
    case 5:
    {
    }
    break;
    case 6:
    {
      unsigned int numeroBlocoProcurado;
      printf("Numero do bloco que deseja procurar: ");
      scanf("%u", &numeroBlocoProcurado);
      procurarBlocoEmArquivoBinario("blocos_minerados.bin", quantidadeDeElementosEscritosNoArquivoBinario, numeroBlocoProcurado);
    }
    break;
    case 7:
    {
      unsigned int enderecoMinerador;
      int quantidadeDeBlocos;
      printf("Numero do minerador: ");
      scanf("%u", &enderecoMinerador);
      printf("Digite a quantidade de blocos minerados por esse minerador\nque voce deseja imprimir: ");
      scanf("%d", &quantidadeDeBlocos);
      procurarNoArquivoDeIndicesMinerador(enderecoMinerador, quantidadeDeBlocos, "arquivo_indices_minerador.bin", quantidadeDeElementosEscritosNoArquivoBinario);
    }
    break;
    case 8:
    {
    }
    break;
    case 9:
    {
      /*
      unsigned int nonceProcurado;
      printf("Digite um nonce: ");
      scanf("%u", &nonceProcurado);
      procurarNonceEmArquivoBinario("arquivo_indices_nonce.bin", quantidadeDeElementosEscritosNoArquivoBinario, nonceProcurado);
      */
    }
    break;
    default:
      printf("Opcao invalida.\n");
      break;
    }
  } while (choice != 0);

  return 0;
}
