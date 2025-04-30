<div align="center">

# Orientações para Clonar o repertório

</div>

## Passo 1 - para computador que não tem o git instalado:

    (1) Crie uma conta no Github, se ainda não tiver: https://github.com

    (2) Instale o git no windows:  

        (2.1) abra o powershell (ou cmd)
        
        (2.2) escreva no prompt: 
            winget install --id Git.Git -e --source winget

    (3) verifique se está instalado coretamente:
        git --version
    
    (4) Crie um diretório (de preferência no raiz)
        md github

    (5) Entre no diretório
        cd github
    
    (6) faça a configuração inicial do github
        git config --global user.name "Seu Nome"
        git config --global user.email "seu@email.com"
    (acho que ele vai pedir a senha em algum momento...)

    
## Passo 2 - para quem já instalou o git na máquina

### Faça o clone do repertório. (só uma vez)

    (1) no diretório que você vai armazenar os repertórios que você clonar digite:
        git clone <endereço do repertório a ser clonado> 
            no caso vc vai digitar assim:
        git clone https://github.com/Cafta/PUC
    
    Agora as pastas estão inteiras no computador

### Alterando e salvando

    (1) Abra a pasta desejada no VSCode ou na IDE do Arduíno (se for código para arduino)

    (2) Faça as alterações que quiser, sem medo de danificar o código (ele ainda está só no seu computador)

    (3) Quando estiver satisfeito, salve sua alteração no repositório
    (para isso tem que estar autorizado por mim)
        (3.1) no powershell ou cmd na pasta correspondente digite:
            git pull
            git add .
            git commit -m "Aqui você faz a descrição do que você alterou no projeto"
            git push
    

