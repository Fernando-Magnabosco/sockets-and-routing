Para compilar:

    $ make

Para executar:

    $ ./router <id>

Onde `<id>` é o identificador do roteador, definido dentro de roteador.config;

Atualmente um roteador pode mandar e receber mensagens para o roteador a sua esquerda ou a sua direita. Por enquanto ele considera o valor da porta e não o ID, então prossiga com cuidado;

Para enviar uma mensagem, digite l ou r para escolher o roteador a quem enviar, digite a mensagem e aperte enter. A mensagem será enviada para o roteador escolhido, que a imprimirá na tela.

Ainda não estão implementados menus mais complexos ou um packet handler, então o programa não faz nada além de enviar e receber mensagens.
