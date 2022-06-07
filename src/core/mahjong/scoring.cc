
#include "core/mahjong/scoring.hpp"
#include <stdexcept>

namespace mj {
namespace scoring {

template<>
Fast8 eval<Yaku::riichi>(ScoringCombo &combo, const Win &win)
{
    combo[Yaku::riichi] = win.check(f_Riichi, fp_Riichi);
    return combo[Yaku::riichi];
}

template<>
Fast8 eval<Yaku::ippatsu>(ScoringCombo &combo, const Win &win)
{
    combo[Yaku::ippatsu] = (bool)win.check(f_Ippatsu);
    return combo[Yaku::ippatsu];
}

template<>
Fast8 eval<Yaku::men_tsumo>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::pinfu>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");   
}

template<>
Fast8 eval<Yaku::ipeikou>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::haitei>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::houtei>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::rinshan>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chankan>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::tanyao>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::hatsu>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chun>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::haku>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::prevailing_wind>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::seat_wind>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chanta>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::sanshoku_seq>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::sanshoku_set>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::ittsu>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::toitoi>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::sanankou>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::sankantsu>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chitoitsu>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::honroutou>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::shousangen>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::honitsu>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::junchan>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::ryanpeikou>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::chinitsu>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}

template<>
Fast8 eval<Yaku::dora>(ScoringCombo &combo, const Win &win)
{
    throw std::logic_error("not implemented yet");
}


} // namespace scoring
} // namespace mj