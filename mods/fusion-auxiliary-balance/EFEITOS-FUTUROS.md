# Contrato de efeitos condicionados — Elegant Egotist

Header público: `origin_effects_v1.h`. API opcional:
`fusion-auxiliary-balance:origin-effects-v1`.

O dono do efeito informa quais cartas têm efeito condicionado ativo. A consulta
é dinâmica: pode depender da configuração do MOD. Não registra todos os monstros.
Um registro por MOD pode reconhecer quantas cartas quiser; há 16 provedores extras.
O catálogo de Fusion/Metalmorph e a consulta ao Ritual MOD não gastam essas vagas.

## Exemplo de integração em um futuro MOD

```c
#include "origin_effects_v1.h"
static const OriginEffectsV1 *egotist;
static int my_enabled(int id) {
    return my_mod_active && my_effect_setting && id == MY_CARD_ID;
}
static void connect_egotist(void) {
    egotist = host->find(host, ORIGIN_EFFECTS_V1_KEY);
    if (egotist && egotist->abi == 1) {
        if (!egotist->register_provider("my-mod-id", my_enabled)) {
            /* Report registration failure; do not claim equip support. */
            egotist = 0;
        }
    } else egotist = 0;
}
static unsigned borrowed_origin(int slot) {
    return egotist ? egotist->lease(slot) : 0;
}
/* In the effect's own eligibility gate: */
/* my_enabled(card_id) && (my_real_condition(slot) || borrowed_origin(slot)) */
```

Chame connect_egotist na inicialização depois que Balanceamento estiver carregado;
se não estiver disponível, tente novamente na aplicação das configurações ou em
um ponto seguro, sem presumir a ordem. Sem a API, o efeito conserva sua condição
original. O callback deve ser puro, rápido e retornar falso com o MOD desativado.
Trocar o callback usando a mesma chave substitui o registro, não duplica.
No shutdown, antes de descarregar o código, use
`egotist->register_provider("my-mod-id", 0)` e descarte o ponteiro.

## O que o novo efeito deve implementar

- Seu comportamento normal e sua condição real continuam pertencendo ao seu MOD.
- `lease(slot)` retorna um serial não zero enquanto a liberação pelo equip estiver
  válida para aquela instância física. Nunca marque uma invocação real falsa.
- Efeitos contínuos consultam a condição/lease; se o lease acaba, o bypass termina.
- Efeitos de entrada devem guardar identidade da carta e serial processado para
  ativar uma vez; respeitar face, fase e demais condições próprias do efeito.
- Não copie o lease para outra carta ou resultado de fusão. Mudanças de controle
  são acompanhadas pela identidade física; remoção/substituição revoga a liberação.
- Registro apenas autoriza o alvo. Não implementa nem dispara automaticamente um
  efeito novo: o código desse efeito deve usar a condição acima.

O MOD de Rituais usa seu catálogo `effect_intelligence_v1` já existente e a ponte
`origin-unlock-v1`; não precisa migrar para a API nova. As duas APIs coexistem.
