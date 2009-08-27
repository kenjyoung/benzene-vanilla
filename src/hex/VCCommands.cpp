//----------------------------------------------------------------------------
/** @file VCCommands.cpp
 */
//----------------------------------------------------------------------------

#include "BoardUtils.hpp"
#include "PlayerUtils.hpp"
#include "VCCommands.hpp"
#include "VCUtils.hpp"

using namespace benzene;

//----------------------------------------------------------------------------

VCCommands::VCCommands(Game& game, HexEnvironment& env)
    : m_game(game), 
      m_env(env)
{
}

VCCommands::~VCCommands()
{
}

void VCCommands::Register(GtpEngine& e)
{
    Register(e, "vc-between-cells", &VCCommands::CmdGetVCsBetween);
    Register(e, "vc-connected-to", &VCCommands::CmdGetCellsConnectedTo);
    Register(e, "vc-get-mustplay", &VCCommands::CmdGetMustPlay);
    Register(e, "vc-intersection", &VCCommands::CmdVCIntersection);
    Register(e, "vc-union", &VCCommands::CmdVCUnion);
    Register(e, "vc-build", &VCCommands::CmdBuildStatic);
    Register(e, "vc-build-incremental", &VCCommands::CmdBuildIncremental);
    Register(e, "vc-undo-incremental", &VCCommands::CmdUndoIncremental);
}

void VCCommands::Register(GtpEngine& engine, const std::string& command,
                          GtpCallback<VCCommands>::Method method)
{
    engine.Register(command, new GtpCallback<VCCommands>(this, method));
}

VC::Type VCCommands::VCTypeArg(const HtpCommand& cmd, std::size_t number) const
{
    return VCTypeUtil::fromString(cmd.ArgToLower(number));
}

//----------------------------------------------------------------------------

/** Builds VCs for both players and displays ice info for the given
    color in the current board-state. 
    Usage: "vc-build [color]"
*/
void VCCommands::CmdBuildStatic(HtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    HexColor color = HtpUtil::ColorArg(cmd, 0);
    HexBoard& brd = m_env.SyncBoard(m_game.Board());
    brd.ComputeAll(color);
    cmd << brd.getInferiorCells().GuiOutput();
    if (!PlayerUtils::IsDeterminedState(brd, color))
    {
        bitset_t consider = PlayerUtils::MovesToConsider(brd, color);
        cmd << BoardUtils::GuiDumpOutsideConsiderSet(brd, consider,
                                              brd.getInferiorCells().All());
    }
    cmd << '\n';
}

/** Builds VCs incrementally for both players and displays ice info
    the given color in the current board-state.
    Usage: "vc-build-incremental [color] [move]"
*/
void VCCommands::CmdBuildIncremental(HtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    HexColor color = HtpUtil::ColorArg(cmd, 0);
    HexPoint point = HtpUtil::MoveArg(cmd, 1);
    HexBoard& brd = *m_env.brd; // <-- NOTE: no call to SyncBoard()!
    brd.PlayMove(color, point);
    cmd << brd.getInferiorCells().GuiOutput();
    if (!PlayerUtils::IsDeterminedState(brd, color))
    {
        bitset_t consider = PlayerUtils::MovesToConsider(brd, color);
        cmd << BoardUtils::GuiDumpOutsideConsiderSet(brd, consider,
                                           brd.getInferiorCells().All());
    }
    cmd << '\n';
}

/** Reverts VCs built incrementally. 
    Usage: "vc-undo-incremental"
*/
void VCCommands::CmdUndoIncremental(HtpCommand& cmd)
{
    UNUSED(cmd);
    m_env.brd->UndoMove();
}

/** Returns a list of VCs between the given two cells.  
    Usage: "vc-between-cells x y c t", where x and y are the cells, c
    is the color of the player, and t is the type of connection
    (full,semi).
*/
void VCCommands::CmdGetVCsBetween(HtpCommand& cmd)
{
    cmd.CheckNuArg(4);
    HexPoint from = HtpUtil::MoveArg(cmd, 0);
    HexPoint to = HtpUtil::MoveArg(cmd, 1);
    HexColor color = HtpUtil::ColorArg(cmd, 2);
    VC::Type ctype = VCTypeArg(cmd, 3);
    HexBoard& brd = *m_env.brd;
    HexPoint fcaptain = brd.GetGroups().CaptainOf(from);
    HexPoint tcaptain = brd.GetGroups().CaptainOf(to);
    std::vector<VC> vc;
    brd.Cons(color).VCs(fcaptain, tcaptain, ctype, vc);
    const VCList& lst = brd.Cons(color).GetList(ctype, fcaptain, tcaptain);
    cmd << '\n';
    std::size_t i = 0;
    for (; i < (std::size_t)lst.softlimit() && i < vc.size(); ++i) 
        cmd << color << " " << vc.at(i) << '\n';
    if (i >= vc.size())
        return;
    cmd << color << " " << fcaptain << " " << tcaptain << " ";
    cmd << "softlimit ----------------------";
    cmd << '\n';
    for (; i < vc.size(); ++i)
        cmd << color << " " << vc.at(i) << '\n';
}

/** Returns a list of cells the given cell shares a vc with.
    Usage: "vc-connected-to x c t", where x is the cell in question,
    c is the color of the player, and t is the type of vc. 
*/
void VCCommands::CmdGetCellsConnectedTo(HtpCommand& cmd)
{
    cmd.CheckNuArg(3);
    HexPoint from = HtpUtil::MoveArg(cmd, 0);
    HexColor color = HtpUtil::ColorArg(cmd, 1);
    VC::Type ctype = VCTypeArg(cmd, 2);
    bitset_t pt = VCSetUtil::ConnectedTo(m_env.brd->Cons(color), 
                                         m_env.brd->GetGroups(), from, ctype);
    cmd << HexPointUtil::ToPointListString(pt);
}

/** Prints the cells in the current mustplay for the given player.
    Usage: "vc-get-mustplay [color]"
*/
void VCCommands::CmdGetMustPlay(HtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    HexColor color = HtpUtil::ColorArg(cmd, 0);
    bitset_t mustplay = VCUtils::GetMustplay(*m_env.brd, color);
    InferiorCells inf(m_env.brd->getInferiorCells());
    inf.ClearVulnerable();
    inf.ClearDominated();
    cmd << inf.GuiOutput();
    if (!PlayerUtils::IsDeterminedState(*m_env.brd, color))
    {
        bitset_t consider = PlayerUtils::MovesToConsider(*m_env.brd, color);
        cmd << BoardUtils::GuiDumpOutsideConsiderSet(*m_env.brd, consider,
                                                     inf.All());
    }
}

/** Prints the cells in the all connections between given endpoints. 
    Usage: "vc-intersection [from] [to] [color] [vctype]"
*/
void VCCommands::CmdVCIntersection(HtpCommand& cmd)
{
    cmd.CheckNuArg(4);
    HexPoint from = HtpUtil::MoveArg(cmd, 0);
    HexPoint to = HtpUtil::MoveArg(cmd, 1);
    HexColor color = HtpUtil::ColorArg(cmd, 2);
    VC::Type ctype = VCTypeArg(cmd, 3);
    HexBoard& brd = *m_env.brd;
    HexPoint fcaptain = brd.GetGroups().CaptainOf(from);
    HexPoint tcaptain = brd.GetGroups().CaptainOf(to);
    const VCList& lst = brd.Cons(color).GetList(ctype, fcaptain, tcaptain);
    bitset_t intersection = lst.hardIntersection();
    cmd << HexPointUtil::ToPointListString(intersection);
}

/** Prints the cells in union of connections between given endpoints. 
    Usage: "vc-union [from] [to] [color] [vctype]"
*/
void VCCommands::CmdVCUnion(HtpCommand& cmd)
{
    cmd.CheckNuArg(4);
    HexPoint from = HtpUtil::MoveArg(cmd, 0);
    HexPoint to = HtpUtil::MoveArg(cmd, 1);
    HexColor color = HtpUtil::ColorArg(cmd, 2);
    VC::Type ctype = VCTypeArg(cmd, 3);
    HexBoard& brd = *m_env.brd;
    HexPoint fcaptain = brd.GetGroups().CaptainOf(from);
    HexPoint tcaptain = brd.GetGroups().CaptainOf(to);
    const VCList& lst = brd.Cons(color).GetList(ctype, fcaptain, tcaptain);
    bitset_t un = lst.getGreedyUnion(); // FIXME: shouldn't be greedy!!
    cmd << HexPointUtil::ToPointListString(un);
}

//----------------------------------------------------------------------------