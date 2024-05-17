import numpy as np
import pandas as pd
import math as m

def distanciaEntreCidades(cidade1, cidade2):
  # aqui calcula a distância através da posição geográfica das cidades
  distancia = m.sqrt((cidade1.latitude - cidade2.latitude)**2+(cidade1.longitude - cidade2.longitude)**2)
  return distancia

def menor(aberta): # aberta é um array de vértices
  menorG = float('inf')
  for vertice in aberta:
    if vertice.G < menorG:
      menorG = vertice.G
      verticeRetorno = vertice
  return verticeRetorno

class Adjacente:
  def __init__(self, vertice, distancia, tempo = 0):
    # self.nome = nome
    self.vertice = vertice
    self.distancia = distancia
    self.tempo = tempo
    velocidade = distancia/(tempo/60) # tempo está em min transformado em horas
    consumo = 3*10**(-9) * (velocidade)**5 - 7*10**(-7)* (velocidade)**4 + 5*10**(-5) * (velocidade)**3 - 0.0001 * (velocidade)**2 + 0.0573 * (velocidade) + 3.5077
    precoGasolina = 5.31
    self.custo = (distancia/consumo) * precoGasolina

class Cidade:
  def __init__(self, nome, latitude, longitude):
    self.nome = nome
    self.latitude = float(latitude)
    self.longitude = float(longitude)
    self.adjacentes = []

class Vertice:
  def __init__(self, rotulo, distancia_objetivo):
    self.rotulo = rotulo
    self.visitado = False
    self.distancia_objetivo = distancia_objetivo
    self.adjacentes = [] # São Adjacentes
    self.G = 0

  def adiciona_adjacente(self, adjacente):
    self.adjacentes.append(adjacente)

  def mostra_adjacentes(self):
    for i in self.adjacentes:
      print(i.vertice.rotulo, i.distancia, i.tempo, i.custo)

class Grafo:
    def __init__(self, dest):
        self.destino = dest  # Destino é um objeto Cidade
        self.verticeDestino = Vertice(dest.nome, 0)
        self.vertices = []  # Dentro de cada Vertice tem os adjacentes.

    def calculaVertices(self):
        print('Criando vértices do grafo.......', end='')
        for i in range(len(cidades)):
            vertice = Vertice(cidades[i].nome, distanciaEntreCidades(cidades[i], self.destino))
            self.vertices.append(vertice)
            if vertice.rotulo == self.destino.nome:
                self.verticeDestino = vertice
        print('OK')

    def adicionarAdjacentes(self):
        print('Adicionando adjacentes........', end='')
        for i in range(len(cidades)):
            for j in range(len(cidades)):
                if not m.isnan(bdDistancia.iloc[i, j]):
                    adjacentei = Adjacente(self.vertices[i], bdDistancia.iloc[i, j], bdTempo.iloc[i, j])
                    adjacentej = Adjacente(self.vertices[j], bdDistancia.iloc[i, j], bdTempo.iloc[i, j])
                    self.vertices[i].adiciona_adjacente(adjacentej)
                    # print(self.vertices[i].rotulo, adjacentej.vertice.rotulo, 'distancia=', adjacentej.distancia, 'Tempo=', adjacentej.tempo)
                    self.vertices[j].adiciona_adjacente(adjacentei)
                    # print(self.vertices[j].rotulo, adjacentei.vertice.rotulo, 'distancia=', adjacentej.distancia, 'Tempo=', adjacentej.tempo)
        print('OK')

class VetorOrdenado:
  def __init__(self, capacidade):
    self.capacidade = capacidade
    self.ultima_posicao = -1
    # Mudança no tipo de dados
    self.valores = np.empty(self.capacidade, dtype=object) # são vértices

  # Referência para o vértice e comparação com a distância para o objetivo
  def insere(self, vertice):
    if self.ultima_posicao == self.capacidade - 1:
      print('Capacidade máxima atingida')
      return
    posicao = 0
    for i in range(self.ultima_posicao + 1):
      posicao = i
      if self.valores[i].distancia_objetivo > vertice.distancia_objetivo:
        break
      if i == self.ultima_posicao:
        posicao = i + 1
    x = self.ultima_posicao
    while x >= posicao:
      self.valores[x + 1] = self.valores[x]
      x -= 1
    self.valores[posicao] = vertice
    self.ultima_posicao += 1

  def imprime(self):
    if self.ultima_posicao == -1:
      print('O vetor está vazio')
    else:
      for i in range(self.ultima_posicao + 1):
        print(i, ' - ', self.valores[i].rotulo, ' - ', self.valores[i].distancia_objetivo)

class Gulosa:
  def __init__(self, objetivo):   # Associa os parâmetros iniciais
    self.objetivo = objetivo      # variável objetivo é um objeto Vertice
    self.encontrado = False       # variável encontrado recebe False (uso interno)

  def buscar(self, atual):      # Única função da classe
    print('-------')                          # Mostra na tela onde está (qual cidade)
    print('Atual: {}'.format(atual.rotulo))   # e a distância para as cidades vizinhas
    atual.visitado = True   # seta visitado para True, evitando que retorne para esta cidade
                            # na próxima cidade a ser avaliada.
                            # perceba que este 'atual' é a cidade passada na busca.

    if atual == self.objetivo:  # Verifica se já chegou na cidade objetivo
      self.encontrado = True
    else:
      vetor_ordenado = VetorOrdenado(len(atual.adjacentes))  # Instancia ("cria") um objeto do tipo
                                                             # tipo 'VetorOrdenado', de tamanho igual
                                                             # ao número de cidades adjacentes
      for adjacente in atual.adjacentes:  # Percorre todas as cidades adjacentes da cidade de
                                          # 'atual' (passada no parâmetro)
        if adjacente.vertice.visitado == False:     # Preenche o vetor ordenado com todos os vertices
          adjacente.vertice.visitado == True        # do adjacente, de forma que o vértice mais próximo
          vetor_ordenado.insere(adjacente.vertice)  # ficará em primeiro da lista
      vetor_ordenado.imprime() # imprime a lista

      if vetor_ordenado.valores[0] != None:     # Para não dar erro, garante que existe a distância
                                                # registrada no vetor
        self.buscar(vetor_ordenado.valores[0])  # avança para o vértice mais próximo (recorrentemente
                                                # até que encontre o destino)


class Dijkastra:
    def __init__(self, grafo, avaliacao='distancia'):  # avaliação é:'distancia', 'tempo' ou 'custo'
        self.objetivo = grafo.verticeDestino  # objetivo é um vértice  ;
        self.avaliacao = avaliacao
        self.encontrado = False
        self.aberta = []
        self.expandida = []

    def buscar(self, origem):  # origem é vértice
        origem.G = 0
        self.aberta.append(origem)
        while len(self.aberta) > 0:
            atual = menor(self.aberta)
            self.aberta.remove(atual)
            self.expandida.append(atual)
            if atual.rotulo == self.objetivo.rotulo:
                self.encontrado = True
                break
            else:
                for adjacente in atual.adjacentes:
                    if not adjacente.vertice in self.expandida:
                        if not adjacente.vertice in self.aberta:
                            # Calculando G para cada um dos adjacentes:
                            if self.avaliacao == 'distancia':
                                adjacente.vertice.G = atual.G + adjacente.distancia
                            elif self.avaliacao == 'tempo':
                                adjacente.vertice.G = atual.G + adjacente.tempo
                            elif self.avaliacao == 'custo':
                                adjacente.vertice.G = atual.G + adjacente.custo
                            # Colocando na lista aberta esse vértice
                            self.aberta.append(adjacente.vertice)
        # acabou o while, significa que percorreu todas as possibilidades
        # agora é achar o melhor caminho
        caminhoInverso = []
        atual = self.objetivo
        while atual.rotulo != origem.rotulo:
            caminhoInverso.append(atual)
            listaVerticesAdjacentes = []
            for adjacente in atual.adjacentes:
                listaVerticesAdjacentes.append(adjacente.vertice)
            atual = menor(listaVerticesAdjacentes)
        caminhoInverso.append(origem)
        caminho = caminhoInverso[::-1]  # inverte o caminho
        # Agora mostra a lista
        for vertice in caminho:
            print(vertice.rotulo)
            if not vertice.rotulo == self.objetivo.rotulo:
                for adjacente in vertice.adjacentes:
                    print(' - ', adjacente.vertice.rotulo, ' - ', adjacente.vertice.G)

# Lendo dados do Banco de Dados 'RegiaoMetropolitanaCampinas.xlsx'
bdCidades = pd.read_excel('RegiaoMetropolitanaCampinas.xlsx', sheet_name='Planilha1', engine='openpyxl')
bdCidades.drop(columns=bdCidades.columns[0], inplace=True)
bdCidades[['latitude', 'longitude']] = bdCidades['coordenadas'].str.split(',', expand=True)
bdDistancia = pd.read_excel('RegiaoMetropolitanaCampinas.xlsx', sheet_name='PlanilhaDistancia', engine='openpyxl')
bdTempo = pd.read_excel('RegiaoMetropolitanaCampinas.xlsx', sheet_name='PlanilhaTempo', engine='openpyxl')
# Removendo a coluna com o nome das cidades, para que o indice x e y correspondam a mesma cidade
bdDistancia.drop('Cidade1',axis=1, inplace=True)
bdTempo.drop('Cidade1',axis=1, inplace=True)

# Criando listagem de cidades, com suas coordenadas GPS
cidades = []
for i in range(0, len(bdCidades)):
  cidades.append(Cidade(bdCidades.cidade[i], bdCidades.latitude[i], bdCidades.longitude[i]))

# Escolha da cidade destino:
menu = ""
i = 0
for cidad in cidades:
  menu += '(' + str(i) + ')'
  i += 1
  menu += str(cidad.nome) + '\n'
cidadeDestino = input(menu+'\nEscolha o número da cidade DESTINO: ')
cidadeDestino = int(cidadeDestino)

# Escolha da cidade de Origem:
menu = ""
i = 0
for cidad in cidades:
  menu += '(' + str(i) + ')'
  i += 1
  menu += str(cidad.nome) + '\n'
cidadeOrigem = input(menu+'\nEscolha o número da cidade de ORIGEM: ')
cidadeOrigem = int(cidadeOrigem)

# Criando o Grafo da cidade destino:
print('Cidade escolhida =', cidades[cidadeDestino].nome, '(Grafo criado)')
grafoEscolha = Grafo(cidades[cidadeDestino])
grafoEscolha.calculaVertices()
grafoEscolha.adicionarAdjacentes()

# TRABALHANDO COM BUSCA GULOSA:
print('\n\n*********** BUSCA GULOSA *************')
busca_gulosa = Gulosa(grafoEscolha.vertices[cidadeDestino])
busca_gulosa.buscar(grafoEscolha.vertices[cidadeOrigem])

# TRABALHANDO COM DIJKASTRA:
# Escolha o método de escolha:
print('\n\n************   ESCOLHA UM MÉTODO DE BUSCA ************')
menu = "1 - Menor Distância \n2 - Menor Tempo\n3 - Menor Curso"
metodo = input(menu+'\nQual o método desejado?  ')
if metodo == '1': metodo = 'distancia'
elif metodo == '2': metodo = 'tempo'
elif metodo == '3': metodo = 'custo'
dijkastra = Dijkastra(grafoEscolha, metodo)
print('\n\n*********** DIJKASTRA *************')
print('Escolhido o método:', metodo, 'de viagem')

dijkastra.buscar(grafoEscolha.vertices[cidadeOrigem])