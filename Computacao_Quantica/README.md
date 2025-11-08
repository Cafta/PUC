# Visualização de Interação por Fase em Circuitos Quânticos

Este projeto é parte da pesquisa realizada para a disciplina de ***Tópicos de Engenharia e Automação II***, com o professor ***Hamilton da Gama Schroder Filho*** e demonstra a visualização e análise de um circuito quântico simples que explora os efeitos de interferência quântica usando portas Hadamard e T.

* *Autor:* ***Carlos Amaral***

## Descrição

O notebook `Interacao_por_fase_Visualizacao.ipynb` implementa um circuito quântico que demonstra:
- Superposição quântica usando portas Hadamard
- Mudanças de fase usando a porta T
- Interferência quântica e seus efeitos nas probabilidades de medição

## Requisitos

- Python 3.8 ou superior
- Qiskit
- Qiskit-Aer
- Matplotlib
- Jupyter/VS Code com suporte a notebooks

## Instalação

As dependências necessárias podem ser instaladas diretamente no notebook usando:

```python
%pip install qiskit
%pip install qiskit-aer
%pip install matplotlib
```

## Como Usar

1. Abra o notebook `Interacao_por_fase_Visualizacao.ipynb` no google Colab, ou se preferir no VS Code, Jupyter ou IDE de sua preferência.
2. Execute todas as células em ordem
3. O notebook irá:
   - Criar um circuito quântico com um qubit
   - Aplicar uma sequência de portas (H → T → H)
   - Simular o circuito
   - Visualizar as probabilidades de medição antes e depois das operações

## Estrutura do Notebook

- **Importação de Bibliotecas**: Configuração inicial do ambiente
- **Criação do Circuito**: Definição do circuito quântico
- **Aplicação de Portas**: Implementação da sequência H-T-H
- **Simulação**: Execução do circuito no simulador Aer
- **Visualização**: Gráficos comparativos das probabilidades
- **Explicação**: Análise detalhada dos resultados

## Resultados

O notebook demonstra como:
- A primeira porta Hadamard cria uma superposição 50/50
- A porta T introduz uma mudança de fase de π/4
- A segunda porta Hadamard causa interferência
- As probabilidades finais são aproximadamente:
  - |0⟩: 85.4%
  - |1⟩: 14.6%

## Conceitos Quânticos Demonstrados

- Superposição quântica
- Mudança de fase
- Interferência quântica
- Medição de estados quânticos
- Efeitos de portas quânticas em sequência

## Extensões Possíveis

- Experimentar com diferentes ângulos de fase
- Adicionar mais qubits
- Testar outras sequências de portas
- Visualizar na esfera de Bloch
- Explorar outros tipos de interferência

## Referências

- [Documentação do Qiskit](https://qiskit.org/documentation/)
- [Tutorial de Portas Quânticas](https://qiskit.org/textbook/ch-gates/quantum-gates.html)
- [Visualização de Estados Quânticos](https://qiskit.org/textbook/ch-states/representing-qubit-states.html)