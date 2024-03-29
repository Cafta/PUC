# -*- coding: utf-8 -*-
"""Everton_AI_03.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1WwJ0b1HYZDH2QE1jKXMfP6l_qhwvRigi

# Laboratório de IA e Automação

## Laboratórios 03

*André Kimaid* RA:21004050

*Carlos Amaral* RA:21986609

*Guilherme Talman* RA:21010098

*Murilo Trevisan* RA:21986260

## Exercício 1:
Escreva uma função em Python que recebe uma string por parâmetro. A função verifica se a string recebida contém a sílaba “Is” no início dela. Se a string fornecida já começar com “Is”, retorne a string inalterada, caso contrário adicione a e retorne a string.

A string é recebida via teclado.
"""

def isCheck(palavra):     # Cria a função que checa Is e retorna
  if palavra[0:2] == 'Is':     # Verifica se a palavra começa com 'Is'
    return palavra
  else:
    return 'Is' + palavra

palavra = input('Escreva uma palavra: ')
print(isCheck(palavra))

palavra = input('Escreva uma palavra: ')
print(isCheck(palavra))

"""## Exercício 2:
Escreva uma função em Python que recebe duas string por parâmetro. A função troca os dois primeiros caracteres de cada string e retorna o resultado. As strings são recebidas via teclado.
"""

def switchFirstSecondChar(word1,word2):
  w1 = word1[:2]                          #pega os dois primeiros caracteres de cada palavra
  w2 = word2[:2]
  result1 = w2 + word1[2:len(word1)]      #junta com o resto da outra palavra
  result2 = w1 + word2[2:len(word2)]
  return result1 + "\n" + result2         # retorna a resposta

word1 = str(input('Digite uma palavra: '))   #Pede a palavra 1
word2 = str(input('Digite outra palavra: ')) #Pede a palavra 2
print(switchFirstSecondChar(word1, word2))   #Printa o valor da funcao

"""## Exercício 3:
Escreva uma função em Python que recebe uma string por parâmetro. A função retorna uma lista de palavras maiores que n. A string e o valor de n é recebida via teclado.
"""

def maioresQue(nLetras, frase):   # Cria a função de análise
  aReturn = []        # Array em branco
  palavras = frase.split(' ')    # Quebra a frase no separador 'espaço em branco'
  for palavra in palavras:      # Percorre cada uma dos itens do array
    if len(palavra) > nLetras:  # verifica se cada item é maior que o número máximo de letra
      aReturn.append(palavra)   # adiciona o item no array
  return aReturn  # retorna o array

nLetras = input('Digite o tamanho de corte: ')
frase = input('Digite a frase para análise: ')
print(maioresQue(int(nLetras), frase))

"""## Exercício 4:
Escreva uma função em Python que recebe duas string por parâmetro. A função troca os dois últimos caracteres de cada string e retorna o resultado. As strings são recebidas via teclado.
"""

def switchFirstSecondChar(word1,word2):
  w1 = word1[-2:]                          #pega os dois primeiros caracteres de cada palavra
  w2 = word2[-2:]
  result1 = word1[0:len((word1))-2] + w2      #junta com o resto da outra palavra
  result2 = word2[0:len((word2))-2] + w1
  return result1 + "\n" + result2         # retorna a resposta

word1 = str(input('Digite uma palavra: ')) #Pede a palavra 1
word2 = str(input('Digite outra palavra: ')) #Pede a palavra 2
print(switchFirstSecondChar(word1, word2)) #Printa o valor da funcao