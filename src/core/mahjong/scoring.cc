
#include "scoring.hpp"
#include <stdexcept>

namespace mj {
namespace scoring {

template<>
Fast8 eval<Yaku::riichi>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    return combo[Yaku::riichi] = hand.check(f_Riichi, fp_Riichi);
}

template<>
Fast8 eval<Yaku::ippatsu>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    return combo[Yaku::ippatsu] = (bool)hand.check(f_Ippatsu);
}

template<>
Fast8 eval<Yaku::men_tsumo>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::pinfu>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");   
}

template<>
Fast8 eval<Yaku::ipeikou>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::haitei>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::houtei>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::rinshan>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chankan>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::tanyao>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::hatsu>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chun>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::haku>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::prevailing_wind>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::seat_wind>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chanta>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::sanshoku_seq>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::sanshoku_set>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::ittsu>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::toitoi>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::sanankou>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::sankantsu>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chitoitsu>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    if (shanten(hand.hand_4hot(), 0, k_ModeChiitoi) == -1)
        return combo[Yaku::chitoitsu] = 2;
    return combo[Yaku::chitoitsu] = 0;

}

template<>
Fast8 eval<Yaku::honroutou>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::shousangen>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::honitsu>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::junchan>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::ryanpeikou>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chinitsu>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::dora>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yakuman::chuuren_poutou>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yakuman::suu_ankou>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yakuman::suu_kantsu>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}


template<>
Fast8 eval<Yakuman::daisangen>(ScoringCombo &combo, const Hand &hand, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

} // namespace scoring
} // namespace mj
