<div align="center">

# Robô seguidor de linha 

***GRUPO 11***  

**PUC CAMPINAS - TURMA 2021**

</div>

Curso: ***Engenharia de Controle e Automação***

---

## Sobre este Projeto

Este projeto desenvolve uma solução, para fins didáticos, de um problema de circulação de escritório. Trata-se de um robô autônomo que se mantém em circulação em um ambiente de trabalho, com intuito de facilitar a entrega de equipamento entre os integrantes da equipe. O robô circulará nas salas, aguardando alguns segundos a cada terminal (mesa). Se alguém necessitar despachar um documento ou equipamento para outra pessoa em outra sala, basta configurar pelo menu (placa BitDogLab) qual o destino, que o robô irá diretamente para o destino entregar a encomenda. Caso ninguém interaja com o robô, após o período de espera ele se dirige para a próxima sala.

***Justificativa***

Este serve como um primeiro passo para se desenvolver sistemas de automatização de entregas. A evolução natural seria fazer o robô se mover de forma autônoma SEM linhas, e com um recipiente fechado para servir de entregas em restaurantes, serviço de quartos em hotéis e outras aplicações do gênero. A comunicação sem fio poderia permitir que o robô, ao invés de ficar circulando sem parar, só fosse até o remetente após sua manifestação em um painel, onde já colocaria o local de destino.

***Estrutura***

Os subdiretórios [\bidDogLab](bitDogLab) e [\Arduino](arduino) são devidos ao fato da programação ser feita individualmente nas duas placas. A comunicação entre elas se dará através do protocolo de comunicação I2C. Mais detalhes sobre cada uma das programações nas suas respectivas pastas.

**Integrantes**
- *Carlos Amaral*
- *Gabriel Morelli*
- *Guilherme Talmann*
- *Murilo Scandiuza* 


## Licença

GPL-3.0