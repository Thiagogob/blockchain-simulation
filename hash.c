// Exemplo de uso da funcao SHA256 da biblioteca Open SSL
//  Este código requer a instalação do pacote libssl-dev
//  Para compilar este código use gcc hash.c -lcrypto

#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h"    //arquivo de definição necessário função SHA256
#include <string.h>
#include "mtwister.h"
#include "mtwister.c"
#include <stdlib.h>

typedef struct BlocoNaoMinerado
{
  unsigned int numero;
  unsigned int nonce;
  unsigned char data[184];
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
} BlocoNaoMinerado;

typedef struct BlocoMinerado
{
  BlocoNaoMinerado bloco;
  unsigned char hash[SHA256_DIGEST_LENGTH];
} BlocoMinerado;

typedef struct listaBTC
{
  unsigned int endereco;
  struct listaBTC *next;
} listaBTC;

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

void insereNoInicio(listaBTC **inicio, unsigned int endereco, int *contador)
{
  listaBTC *novoEndereco = criaNo(endereco);
  if (inicio == NULL)
  {
    *inicio = novoEndereco;
  }
  else
  {
    novoEndereco->next = *inicio;
    *inicio = novoEndereco;
    (*contador)++;
  }
}

void mostraLista(listaBTC *inicio)
{
  listaBTC *aux = inicio;
  while (aux != NULL)
  {
    printf("%u - ", aux->endereco);
    aux = aux->next;
  }
  printf("\n");
}

void printHash(unsigned char hash[], int length)
{
  int i;

  for (i = 0; i < length; ++i)
    printf("%02x", hash[i]);

  printf("\n");
}

/*
unsigned char gerarNumeroAleatorio(int valorMaximo){
    MTRand r = seedRand(1234567);
    unsigned char numeroAleatorio = genRandLong(&r)%valorMaximo;
    return numeroAleatorio;
}
*/

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

void inicializaCarteira(unsigned int *carteira)
{
  for (int i = 0; i < 256; i++)
  {
    carteira[i] = 0;
  }
}

BlocoMinerado minerarBlocoGenesis(BlocoNaoMinerado *blocoGenesis, unsigned int *carteira, listaBTC **enderecosComBTC, MTRand *r, int *contador)
{

  // vetor que armazenará o resultado do hash. Tamanho definidio pela libssl
  unsigned char hash[SHA256_DIGEST_LENGTH];

  // PROCESSO PARA DEFINIR NUMERO ALETORIO QUE REPRESENTA O MINERADOR DO BLOCO:
  unsigned char minerador = genRandLong(r) % 256;
  blocoGenesis->data[183] = minerador;

  SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);

  while (hash[0] != 0)
  {
    blocoGenesis->nonce++;
    SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);
  }

  // Agora que o bloco foi minerado
  // Vamos passar ele para a struct de bloco minerado
  BlocoMinerado *blocoGenesisMinerado = malloc(sizeof(BlocoMinerado));

  // os campos do bloco nao minerado sao enviados para o bloco minerado
  blocoGenesisMinerado->bloco = *blocoGenesis;

  // passando o hash que valida o bloco para a struct de bloco minerado
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    blocoGenesisMinerado->hash[i] = hash[i];
  }

  /*
  printf("Hash: ");
  printHash(hash, SHA256_DIGEST_LENGTH);
  */

  // ATUALIZA CARTEIRA DO SISTEMA DEPOIS DA VALIDAÇÃO DO BLOCO
  carteira[minerador] = 50;

  // COLOCA PRIMEIRO MINERADOR NA LISTA DE ENDERECOS COM BTC
  insereNoInicio(enderecosComBTC, minerador, contador);
  /*
  insereNoInicio(enderecosComBTC, (unsigned char)100, contador);
  insereNoInicio(enderecosComBTC, (unsigned char)50, contador);
  insereNoInicio(enderecosComBTC, (unsigned char)20, contador);
  insereNoInicio(enderecosComBTC, (unsigned char)70, contador);
  */

  //mostraLista(*enderecosComBTC);
  // printf("contador: %d\n", *contador);

  return *blocoGenesisMinerado;
}

unsigned char procuraEnderecoDestino(listaBTC *enderecosComBTC, int indice)
{
  listaBTC *tmp = enderecosComBTC;
  for (int i = 0; i < indice; i++)
  {
    tmp = tmp->next;
  }
  return tmp->endereco;
}

void removerEndereco(listaBTC **enderecosComBTC, unsigned char endereco, int *contador){

  listaBTC *tmp = *enderecosComBTC;
  listaBTC *anterior = NULL;

  if(tmp != NULL && tmp->endereco == endereco){
    *enderecosComBTC = tmp->next;
    free(tmp);
    (*contador)--;
    return;
  }

  while(tmp != NULL && tmp->endereco != endereco){
    anterior = tmp;
    tmp = tmp->next;
  }

  if(tmp!=NULL){
    anterior->next = tmp->next;
    free(tmp);
    (*contador)--;
  }

  return;

}

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

  
  printf("Quantidade de transacoes no bloco: %u\n", qtdTransacoes);
  // definindo os elementos das transacoes
  if(numeroBloco == 2){
    for(int i=0; i<qtdTransacoes; i++){
      for(int j=0; j<3; j++){

        blocoN->data[(i * 3) + j] = 140;
        j++;

        // gerar endereco de destino
        blocoN->data[(i * 3) + j] = genRandLong(r) % 256;
        j++;


        // definir quantidade de bitcoins
        unsigned char qtdBTC = (genRandLong(r) % (carteira[140]+1));
        blocoN->data[(i * 3) + j] = qtdBTC;

        //atualizando carteira para nao haver inconsistencia
        //por exemplo um endereco que possui 10BTC estar repassar 20BTC
        //printf("quantidade na carteira[140]: %u\n", carteira[140]);
        carteira[140] = carteira[140] - qtdBTC;        
      }
    }
  }
  else{
  for (int i = 0; i < qtdTransacoes; i++)
  {
    for (int j = 0; j < 3; j++)
    {

      // gerar endereco de origem
      int indice = genRandLong(r) % (*contador);
      unsigned char endOrigem = procuraEnderecoDestino(enderecosComBTC, indice);
      blocoN->data[(i * 3) + j] = endOrigem;
      j++;

      // gerar endereco de destino
      blocoN->data[(i * 3) + j] = genRandLong(r) % 256;
      j++;

      // definir quantidade de bitcoins
      unsigned char qtdBTC = genRandLong(r) % carteira[endOrigem];
      blocoN->data[(i * 3) + j] = qtdBTC;

      //atualizando carteira para nao haver inconsistencia
      //por exemplo um endereco que possui 10BTC estar repassar 20BTC
      carteira[endOrigem] = carteira[endOrigem] - qtdBTC;
      
    }
  }
  }

  // decidindo quem será o minerador do bloco
  unsigned char minerador = genRandLong(r) % 256;
  blocoN->data[183] = minerador;

}

BlocoMinerado* minerarBloco(BlocoNaoMinerado *blocoN, unsigned int *carteira, listaBTC **enderecosComBTC, int *contador, unsigned char qtdTransacoes){

  BlocoMinerado* blocoNMinerado = malloc(sizeof(BlocoMinerado));

  

  unsigned char hash[SHA256_DIGEST_LENGTH];

  SHA256((unsigned char *)blocoN, sizeof(BlocoNaoMinerado), hash);

  while(hash[0]!=0){
    blocoN->nonce++;
    SHA256((unsigned char *)blocoN, sizeof(BlocoNaoMinerado), hash);
  }

  for(int i =0; i<SHA256_DIGEST_LENGTH; i++){
    blocoNMinerado->hash[i] = hash[i];
  }

  unsigned char minerador = blocoN->data[183];
  carteira[minerador] += 50;

  insereNoInicio(enderecosComBTC, minerador, contador);

  blocoNMinerado->bloco = *blocoN;

  
  for(int i=0; i<qtdTransacoes;i++){
    for(int j=0; j<3; j++){

      //agora que o bloco foi validado
      //devemos atualizar os saldos
      //dos enderecos que receberam BTC
      if(j==1){
        //printf("indice: %u\n", blocoN->data[(i*3)+1]);
        carteira[blocoN->data[(i*3)+1]] += blocoN->data[(i*3)+2];

        if(blocoN->data[(i*3)+2] != 0){
          insereNoInicio(enderecosComBTC, blocoN->data[(i*3)+1], contador);
        }
      }

    }
  }

  //antes de retornar o bloco minerado
  //vamos verificar a necessidade de remover
  //algum endereco da lista de enderecos com BTC
  //afinal, alguem pode ter doado todas as bitcoins que tinha num bloco
  listaBTC* tmp = *enderecosComBTC;
  while(tmp!=NULL){
    if(carteira[tmp->endereco] == 0){
      removerEndereco(enderecosComBTC, tmp->endereco, contador);
    }
    tmp = tmp->next;
  }
   
  

  return blocoNMinerado;
}

int main(int argc, char *argv[])
{
  MTRand r = seedRand(1234567);
  int contador = 0;
  // instanciando e preenchendo bloco genesis
  // da maneira solicitada no enunciado
  BlocoNaoMinerado *blocoGenesis = malloc(sizeof(BlocoNaoMinerado));
  preencheBlocoGenesis(blocoGenesis);

  // inicializando carteira conforme pedido no enunciado
  unsigned int carteira[256];
  inicializaCarteira(carteira);

  // printf("CARTEIRA[122]: %u\n", carteira[122]);

  // cria lista dos enderecos com BTC
  listaBTC *enderecosComBTC = NULL;

  // cria vetor de 16 posições para armazenar blocos minerados
  BlocoMinerado vetorBlocosMinerados[16];

  // minerando o bloco genesis
  BlocoMinerado blocoGenesisMinerado = minerarBlocoGenesis(blocoGenesis, carteira, &enderecosComBTC, &r, &contador);

  // Minera 30.000 blocos agora
  // BlocoMinerado blocoAnterior;
  /*
  for(int i = 0; i < 3; i++){

    //caso especial, onde o primeiro indice do vetor
    //eh o bloco genesis
    if(i==0){
      vetorBlocosMinerados[i] = blocoGenesisMinerado;
      inicializarBloco(vetorBlocosMinerados[(i-1)%16].hash, blocoN, i, &r, &contador, enderecosComBTC, carteira);
    }

    else{

       //alocando memoria para o bloco
       BlocoNaoMinerado* blocoN = malloc(sizeof(BlocoNaoMinerado));

       //chamando uma funcao para preencher o bloco
       //passando o hash anterior + o bloco a ser minerado + i
       inicializarBloco(vetorBlocosMinerados[(i-1)%16].hash, blocoN, i, &r, &contador, enderecosComBTC, carteira);


    }

  }
  */
  BlocoNaoMinerado* blocoN = malloc(sizeof(BlocoNaoMinerado));

  vetorBlocosMinerados[0] = blocoGenesisMinerado;

  // decidindo quantidade de transacoes no bloco
  unsigned char qtdTransacoes = genRandLong(&r) % 62;

  inicializarBloco(vetorBlocosMinerados[0].hash, blocoN, 2, &r, &contador, enderecosComBTC, carteira, qtdTransacoes);

  BlocoMinerado *blocoNMinerado = minerarBloco(blocoN, carteira, &enderecosComBTC, &contador, qtdTransacoes);

  for (int i = 0; i < 184; i++)
  {
    printf("[%u] - ", blocoN->data[i]);
    if ((i + 1) % 3 == 0)
    {
      printf("\n");
    }
  }
  printf("Hash anterior: ");
  printHash(blocoN->hashAnterior, SHA256_DIGEST_LENGTH);
  printf("Hash valido: ");
  printHash(blocoNMinerado->hash, SHA256_DIGEST_LENGTH);
  printf("\nNonce: %d\n", blocoN->nonce);
  printf("Minerador: %u\n", blocoN->data[183]);
  printf("lista de enderecos com BTC: ");
  mostraLista(enderecosComBTC);
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

  //printf("Nonce: %d\n", blocoGenesisMinerado.bloco.nonce);
  //printf("Minerador: %u\n", blocoGenesisMinerado.bloco.data[183]);
  // printf("aleatorio teste: %lu", genRandLong(&r)%61);
  // printf("Hash: ");
  // printHash(hash, SHA256_DIGEST_LENGTH);
  // printf("Carteira[140]: %u", carteira[140]);
  // printf("hash bloco genesis minerado: ");
  // printHash(blocoGenesisMinerado.hash, SHA256_DIGEST_LENGTH);

  // mostraLista(enderecosComBTC);

  // MTRand r = seedRand(1234567);
  /*
  unsigned char vetorLoco[9];
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {

      unsigned char endO = genRandLong(&r) % 256;
      printf("endereco de origem: %u\n", endO);
      vetorLoco[(i * 3) + j] = endO;
      j++;

      unsigned char endD = genRandLong(&r) % 256;
      printf("endereco de destino: %u\n", endD);
      vetorLoco[(i * 3) + j] = endD;
      j++;

      printf("quantidade de BTC: %u\n", 50);
      vetorLoco[(i * 3) + j] = 50;
    }
  }
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
  //mostraLista(enderecosComBTC);
  //insereNoInicio(&enderecosComBTC, (unsigned char)100, &contador);
  //insereNoInicio(&enderecosComBTC, (unsigned char)50, &contador);
  //insereNoInicio(&enderecosComBTC, (unsigned char)20, &contador);
  //insereNoInicio(&enderecosComBTC, (unsigned char)70, &contador);
  
  //mostraLista(enderecosComBTC);
  //printf("qtd elementos: %d", contador);

  //removerEndereco(&enderecosComBTC, 70, &contador);
  //removerEndereco(&enderecosComBTC, 140, &contador);

  //mostraLista(enderecosComBTC);

  //printf("qtd elementos: %d", contador);
  return 0;
}
