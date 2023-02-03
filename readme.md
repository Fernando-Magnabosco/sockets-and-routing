Para compilar:

    $ make

Para executar:

    $ ./router <id>

Onde `<id>` é o identificador do roteador, definido dentro de roteador.config;
Você também pode executar a rede inteira usando o comando:

    $ ./init_all.sh


Este sistema é responsável por fazer todo o esquema de roteamento, evitando contagens ao infinito. É possível enviar mensagens para todos os destinos alcançáveis e o histórico de mensagens é mantido para referência futura. Além disso, um log do sistema está disponível para acompanhar o funcionamento.

Quando um vizinho desliga, o sistema é capaz de se adaptar automaticamente, garantindo que o roteamento continue sem interrupções.


Eduardo Rostirola Barcaroli e Fernando Schreiner Magnabosco
