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


//lista que vai conter os enderecos que possuem BTC
typedef struct listaBTC
{
  unsigned int endereco;
  struct listaBTC *next;
} listaBTC;


//Funcao para criar um no na lista dos enderecos que tem BTC
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


//Funcao para inserir um endereco na lista dos que tem BTC
void insereNoInicio(listaBTC **inicio, unsigned int endereco, int *contador)
{
  listaBTC *novoEndereco = criaNo(endereco);
  if (*inicio == NULL)
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

//essa funcao serve para verificar se o minerador pode entrar na lista
//pois nao eh interessante a repeticao de enderecos na lista
int verificaSeMineradorPodeEntrarNaLista(listaBTC* enderecosComBTC, unsigned int endereco){
  listaBTC *tmp = enderecosComBTC;
  while(tmp !=NULL){
    if(tmp->endereco == endereco){
      return 0;
    }
    tmp = tmp->next;
  }
  return 1;
}

//Funcao para printar a lista
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

//printar hash
void printHash(unsigned char hash[], int length)
{
  int i;

  for (i = 0; i < length; ++i)
    printf("%02x", hash[i]);

  printf("\n");
}

void printarHashArquivo(FILE *pArq, unsigned char* hash){
  const char* FORMATO_BLOCO_MINERADO_HASH = "%02x";
  for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
    fprintf(pArq, FORMATO_BLOCO_MINERADO_HASH, hash[i]);
  }
}

//funcao para preencher o bloco genesis com os campos
//solicitados no enunciado do projeto
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

//inicializar a carteira do projeto com zeros
void inicializaCarteira(unsigned int *carteira)
{
  for (int i = 0; i < 256; i++)
  {
    carteira[i] = 0;
  }
}

//funcao para minerar bloco genesis
BlocoMinerado minerarBlocoGenesis(BlocoNaoMinerado *blocoGenesis, unsigned int *carteira, listaBTC **enderecosComBTC, MTRand *r, int *contador)
{

  // vetor que armazenará o resultado do hash. Tamanho definidio pela libssl
  unsigned char hash[SHA256_DIGEST_LENGTH];

  // PROCESSO PARA DEFINIR NUMERO ALETORIO QUE REPRESENTA O MINERADOR DO BLOCO:
  unsigned char minerador = genRandLong(r) % 256;
  blocoGenesis->data[183] = minerador;

  //gerando um 1° hash para o bloco
  SHA256((unsigned char *)blocoGenesis, sizeof(BlocoNaoMinerado), hash);

  //gerando um novo hash e atualizando o nonce
  //ate que hajam 2 zeros hexadecimais no comeco
  //esse eh o criterio para validar um hash
  //assim sabemos se um bloco foi minerado
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

  return blocoGenesisMinerado;
}

//funcao para procurar um endereco de destino na lista de quem tem BTC
unsigned char procuraEndereco(listaBTC *enderecosComBTC, int indice)
{
  listaBTC *tmp = enderecosComBTC;
  for (int i = 0; i < indice; i++)
  {
    tmp = tmp->next;
  }
  return tmp->endereco;
}

//funcao para remover um endereco
//usada caso algum endereco que tinha BTC tenha transferido todas que tinha
void removerEndereco(listaBTC **enderecosComBTC, unsigned char endereco, int *contador){

  listaBTC *tmp = *enderecosComBTC;
  listaBTC *anterior = NULL;

  if(tmp != NULL && tmp->endereco == endereco){
    *enderecosComBTC = tmp->next;
    //free(tmp);
    (*contador)--;
    return;
  }

  while(tmp != NULL && tmp->endereco != endereco){
    anterior = tmp;
    tmp = tmp->next;
  }

  if(tmp!=NULL){
    anterior->next = tmp->next;
    //free(tmp);
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

  
  //printf("Quantidade de transacoes no bloco: %u\n", qtdTransacoes);
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
      unsigned char endOrigem = procuraEndereco(enderecosComBTC, indice);
      blocoN->data[(i * 3) + j] = endOrigem;
      j++;

      // gerar endereco de destino
      blocoN->data[(i * 3) + j] = genRandLong(r) % 256;
      j++;

      // definir quantidade de bitcoins
      unsigned char qtdBTC = genRandLong(r) % (carteira[endOrigem]+1);
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

BlocoMinerado minerarBloco(BlocoNaoMinerado *blocoN, unsigned int *carteira, listaBTC **enderecosComBTC, int *contador, unsigned char qtdTransacoes){

  BlocoMinerado blocoNMinerado;

  
  unsigned char hash[SHA256_DIGEST_LENGTH];

  //gerando um hash para o bloco
  SHA256((unsigned char *)blocoN, sizeof(BlocoNaoMinerado), hash);


  //alterando hash e nonce ate
  //o hash ser valido
  while(hash[0]!=0){
    blocoN->nonce++;
    SHA256((unsigned char *)blocoN, sizeof(BlocoNaoMinerado), hash);
  }

  //preenchendo campo hash do bloco minerado com o hash valido
  for(int i =0; i<SHA256_DIGEST_LENGTH; i++){
    blocoNMinerado.hash[i] = hash[i];
  }

  //atribuindo as 50 BTC para o minerador do bloco
  unsigned char minerador = blocoN->data[183];
  carteira[minerador] += 50;

  //inserindo minerador na lista dos que tem BTC
  //caso ele nao esteja
  if(verificaSeMineradorPodeEntrarNaLista(*enderecosComBTC, minerador)){
    insereNoInicio(enderecosComBTC, minerador, contador);
  }
  

  //repassando campos do blocoN nao minerado
  blocoNMinerado.bloco = *blocoN;

  //atualizando a carteira apos validada as transacoes
  for(int i=0; i<qtdTransacoes;i++){
    for(int j=0; j<3; j++){

      //agora que o bloco foi validado
      //devemos atualizar os saldos
      //dos enderecos que receberam BTC
      if(j==1){
        //printf("indice: %u\n", blocoN->data[(i*3)+1]);
        carteira[blocoN->data[(i*3)+1]] += blocoN->data[(i*3)+2];

        if(blocoN->data[(i*3)+2] != 0){
          if(verificaSeMineradorPodeEntrarNaLista(*enderecosComBTC, blocoN->data[(i*3)+1])){
            insereNoInicio(enderecosComBTC, blocoN->data[(i*3)+1], contador);
          }
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

void criarArquivoIndicesNonce(BlocoMinerado *vetorBlocosMinerados, int tamanho, const char *nomeArquivo)
{
  FILE *arquivo = fopen(nomeArquivo, "w");
  if (arquivo == NULL)
  {
    printf("Erro ao criar o arquivo de índices por nonce.\n");
    return;
  }

  for (int i = 0; i < tamanho; i++)
  {
    fprintf(arquivo, "%d %u\n", i, vetorBlocosMinerados[i].bloco.nonce);
  }

  fclose(arquivo);
}

void criarArquivoBlocosMinerados(BlocoMinerado *blocos, int quantidade, const char *nomeArquivo)
{
  FILE *arquivo = fopen(nomeArquivo, "wb");
  if (arquivo == NULL)
  {
    printf("Erro ao criar o arquivo de blocos minerados.\n");
    return;
  }

  // Escreve a quantidade de blocos no arquivo
  fwrite(&quantidade, sizeof(int), 1, arquivo);

  // Escreve cada bloco no arquivo
  fwrite(blocos, sizeof(BlocoMinerado), quantidade, arquivo);

  fclose(arquivo);

}

void escreverArquivoTexto(BlocoMinerado *vetorBlocosMinerados){
  const char* FORMATO_BLOCO_MINERADO = "\nBloco: %d\nNonce: %d\nMinerador: %u\n";
  const char* FORMATO_BLOCO_MINERADO_DATA = "[%u] -";
  FILE *pArqTexto = fopen("blocos_minerados.txt", "a+");
  if (pArqTexto == NULL)
  {
    printf("Erro ao criar/abrir arquivo texto de blocos minerados.\n");
    return;
  }
  for(int i=0; i<16; i++){
    fprintf(pArqTexto, FORMATO_BLOCO_MINERADO, vetorBlocosMinerados[i].bloco.numero, vetorBlocosMinerados[i].bloco.nonce, vetorBlocosMinerados[i].bloco.data[183]);
    fprintf(pArqTexto, "Hash anterior: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].bloco.hashAnterior);
    fprintf(pArqTexto, "\n");
    fprintf(pArqTexto, "Hash valido: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].hash);
    fprintf(pArqTexto, "\n");
    for(unsigned char j=0; j<184; j++){
      fprintf(pArqTexto, FORMATO_BLOCO_MINERADO_DATA, vetorBlocosMinerados[i].bloco.data[j]);
      
      if((j+1)%3==0){
        fprintf(pArqTexto, "\n");
      }
      
    }
    fprintf(pArqTexto, "\n=======================================");
  }
  fclose(pArqTexto);
}

void escreverArquivoTextoComGenesis(BlocoMinerado *vetorBlocosMinerados){
  const char* FORMATO_BLOCO_MINERADO = "Bloco: %d\nNonce: %d\nMinerador: %u\n";
  const char* FORMATO_BLOCO_MINERADO_DATA = "[%u] -";
  const char* FORMATO_BLOCO_MINERADO_DATA_GENESIS = "%c";
  FILE *pArqTexto = fopen("blocos_minerados.txt", "a+");
  if (pArqTexto == NULL)
  {
    printf("Erro ao criar/abrir arquivo texto de blocos minerados.\n");
    return;
  }
  for(int i=0; i<16; i++){
    fprintf(pArqTexto, FORMATO_BLOCO_MINERADO, vetorBlocosMinerados[i].bloco.numero, vetorBlocosMinerados[i].bloco.nonce, vetorBlocosMinerados[i].bloco.data[183]);
    fprintf(pArqTexto, "Hash anterior: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].bloco.hashAnterior);
    fprintf(pArqTexto, "\n");
    fprintf(pArqTexto, "Hash valido: ");
    printarHashArquivo(pArqTexto, vetorBlocosMinerados[i].hash);
    fprintf(pArqTexto, "\n");
    for(unsigned char j=0; j<184; j++){
      if(i==0){
        fprintf(pArqTexto, FORMATO_BLOCO_MINERADO_DATA_GENESIS, vetorBlocosMinerados[i].bloco.data[j]);
      }
      else{
        fprintf(pArqTexto, FORMATO_BLOCO_MINERADO_DATA, vetorBlocosMinerados[i].bloco.data[j]);
        if((j+1)%3==0){
          fprintf(pArqTexto, "\n");
        }
      }
    }
    fprintf(pArqTexto, "\n=======================================\n");
  }
  fclose(pArqTexto);
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
//função para imprimir o conteúdo da carteira, mas é apenas teste, depois será excluída
void imprimirCarteira(unsigned int *carteira) {
  for (int i = 0; i < 256; i++) {
    if (carteira[i] > 0) {
      printf("Endereço %d: %u BTC\n", i, carteira[i]);
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  MTRand r = seedRand(1234567);
  int contador = 0;
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
  // Minerar 30.000 blocos agora
  for (int i = 0; i < 46; i++)
  {

    BlocoNaoMinerado blocoN;
    

    // decidindo quantidade de transacoes no bloco
    unsigned char qtdTransacoes = genRandLong(&r) % 62;

    if(i==0){
      inicializarBloco(blocoGenesisMinerado.hash, &blocoN, 2, &r, &contador, enderecosComBTC, carteira, qtdTransacoes);
    }
    else{

      inicializarBloco(hashAnterior, &blocoN, i+2, &r, &contador, enderecosComBTC, carteira, qtdTransacoes);
    }

    BlocoMinerado blocoNMinerado = minerarBloco(&blocoN, carteira, &enderecosComBTC, &contador, qtdTransacoes);


    
    vetorBlocosMinerados[(i+1)%16] = blocoNMinerado;
    
    
    for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
      hashAnterior[i]=blocoNMinerado.hash[i];
    }
    //printf("i: %d\n", i);

    //verificando se eh para escrever no arquivo
    if((i+1)%15==0 && i==14){
      //printf("\nta na hora de escrever no arquivo\n");
      escreverArquivoTextoComGenesis(vetorBlocosMinerados);
    }
    else if((i)%15==0 && i!=15 && i!=0){
      //printf("\nvalor do i: %d\n", i);
      escreverArquivoTexto(vetorBlocosMinerados);
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
    
    
    printf("\n");
    printf("Bloco: %d\n", blocoN.numero);
    printf("Nonce: %d\n", blocoN.nonce);
    printf("Minerador: %u\n", blocoN.data[183]);
    printf("Hash anterior: ");
    printHash(blocoN.hashAnterior, SHA256_DIGEST_LENGTH);
    printf("Hash valido: ");
    printHash(blocoNMinerado.hash, SHA256_DIGEST_LENGTH);
    //printf("lista de enderecos com BTC: ");
    //mostraLista(enderecosComBTC);
    //printf("\n================================\n");
    //printf("pode ? %d", verificaSeMineradorPodeEntrarNaLista(enderecosComBTC, 139));
    
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

  //------------------------ FUNÇÕES QUE CRIAM ARQUIVOS QUE SERÃO UTILIZADOS NOS CASES 6,7,8 e 9 -----------------------------

  printf("\n-----------------------------------------------------------------\n\n");
  criarArquivoIndices(enderecosComBTC, "indices.txt");
  printf("Arquivo de ÍNDICES criado com sucesso.\n");

  criarArquivoIndicesNonce(vetorBlocosMinerados, 16, "indices_nonce.txt");
  printf("Arquivo de ÍNDICES NONCE criado com sucesso.\n");

  //case 6 e 8 utilizarão o arquivo gerado nessa função
  criarArquivoBlocosMinerados(vetorBlocosMinerados, 16, "blocos_minerados.bin");
  //printf apenas para "validar" e verificar o funcionamento da função
  printf("Arquivo de BLOCOS MINERADOS criado com sucesso.\n");
  printf("\n");

  //---------------------------------------------------------------------------------------------------------------------------

  int choice;
    do {
        printf("--------------------------- MENU ---------------------------------------");
        printf("\n0. Inicializar todo o programa\n");
        printf("1. Endereco com mais bitcoins\n");
        printf("2. Endereco que minerou mais blocos\n");
        printf("3. Bloco com mais transacoes\n");
        printf("4. Bloco com menos transacoes\n");
        printf("5. Quantidade media de bitcoins por bloco\n");
        printf("6. Imprimir campos de um bloco\n");
        printf("7. Imprimir campos dos primeiros blocos de um endereco\n");
        printf("8. Imprimir campos dos primeiros blocos em ordem crescente de transacoes\n");
        printf("9. Imprimir campos de todos os blocos com um dado nonce\n");
        //criando um case 10, só pra testar e verificar algumas coisas sobre a carteira
        printf("10. Imprimir carteira\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &choice);

        switch(choice) {
            case 0:{
                
                }break;
            case 1:{

                }break;
            case 2:{
                printf("\n");
                imprimirEnderecoMaisMinerou(enderecosComBTC);
                printf("\n");
                }break;
            case 3:{

                }break;
            case 4:{

                }break;
            case 5:{

                }break;
            case 6:{

                }break;
            case 7:{

                }break;
            case 8:{

                }break;
            case 9:{

                }break;
          case 10:{
              printf("\n->> CONTEÚDO DA CARTEIRA:\n");
              imprimirCarteira(carteira);
              printf("\n");
                }break;
            default:
                printf("Opcao invalida.\n");
                break;
        }
    } while(choice != 0);


  return 0;
}
