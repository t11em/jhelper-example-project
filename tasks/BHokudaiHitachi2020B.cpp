#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <queue>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <list>
#include <random>
#include <sstream>

#define rep(i, m, n) for (int i = (int)(m); i < (int)(n); ++i)
#define rrep(i, m, n) for (int i = (int)(m); i > (int)(n); --i)

using namespace std;

#ifndef DEBUG_OUTPUT
#define DEBUG_OUTPUT
#endif

ofstream ofs("error.log");
template <typename T, typename U>
pair<T, U> tp(T a, U b) {
    return std::make_pair(a, b);
}

constexpr int MOD = 1000000007;

FILE* log_dest = stderr;
using namespace std;
struct GraphData {
    constexpr static size_t                    MaxDegree = 5;
    size_t                                     V, E;
    std::map<size_t, std::map<size_t, size_t>> edges;
    GraphData(std::istream& src) {
        src >> V >> E;
        for (size_t i = 0; i < E; ++i) {
            size_t u, v, d;
            src >> u >> v >> d;
            --u, --v;
            edges[u][v] = d;
            edges[v][u] = d;
        }
        for ([[maybe_unused]] auto& [u, connects] : edges) {
        }
    }
};
struct GridData {
    size_t                        DayType;
    size_t                        N_div, N_pattern, sigma_ele, Delta_event;
    double                        p_event;
    std::vector<std::vector<int>> pw_predict;
    size_t                        N_grid, C_grid_init, C_grid_max, V_grid_max;
    std::vector<size_t>           x, pattern;
    GridData()                 = default;
    GridData(const GridData&) = default;
    GridData(std::istream& src) {
        src >> DayType;
        src >> N_div >> N_pattern >> sigma_ele >> p_event >> Delta_event;
        pw_predict.resize(N_pattern);
        for (size_t i = 0; i < N_pattern; ++i) {
            pw_predict[i].resize(N_div);
            for (size_t j = 0; j < N_div; ++j) {
                src >> pw_predict[i][j];
            }
        }
        src >> N_grid >> C_grid_init >> C_grid_max >> V_grid_max;
        x.resize(N_grid);
        pattern.resize(N_grid);
        for (size_t i = 0; i < N_grid; ++i) {
            src >> x[i] >> pattern[i];
            --x[i];
            --pattern[i];
        }
    }
    GridData& operator=(const GridData&) = default;
};
struct EvData {
    size_t              N_EV, C_EV_init, C_EV_max, V_EV_max, N_trans_max, Delta_EV_move;
    std::vector<size_t> pos;
    std::vector<size_t> now_charge;
    EvData(std::istream& src) {
        src >> N_EV >> C_EV_init >> C_EV_max >> V_EV_max >> N_trans_max >> Delta_EV_move;
        pos.resize(N_EV);
        now_charge.resize(N_EV);
        for (size_t i = 0; i < N_EV; ++i) {
            src >> pos[i];
            --pos[i];
        }
    }
};
struct A {
    GraphData  graph_data;
    GridData   grid_data;
    EvData     ev;
    double     gamma;
    size_t     T_max;
    A(std::istream& src) : graph_data(src), grid_data(src), ev(src) {
        for (size_t i = 0; i < grid_data.N_grid; ++i) {
        }
        src >> gamma;
        src >> T_max;
    }
};
struct B {
    GraphData      graph_data;
    GridData       grid_data;
    EvData         ev_data;
    double     p_const_trans;
    size_t     T_last;
    size_t     P_trans;
    double     gamma;
    int        S_ref_ele, S_ref_trans;
    size_t     T_max;
    B(std::istream& src) : graph_data(src), grid_data(src), ev_data(src) {
        for (size_t i = 0; i < grid_data.N_grid; ++i) {
        }
        src >> p_const_trans >> T_last;
        src >> P_trans >> gamma >> S_ref_ele >> S_ref_trans;
        src >> T_max;
    }
};
struct CarInfo {
    size_t              charge;
    size_t              u, v, dist_from_u, dist_to_v;
    size_t              N_adj;
    std::vector<size_t> a;
    size_t              N_order;
    std::vector<size_t> o;
    void load(std::istream& src, [[maybe_unused]] size_t C_EV_max = 25000, [[maybe_unused]] size_t V = 225,
              [[maybe_unused]] size_t MaxDegree = 5, [[maybe_unused]] size_t N_trans_max = 4,
              [[maybe_unused]] size_t T_last = 900) {
        src >> charge;
        src >> u >> v >> dist_from_u >> dist_to_v;
        --u, --v;
        src >> N_adj;
        a.resize(N_adj);
        for (size_t i = 0; i < N_adj; ++i) {
            src >> a[i];
            --a[i];
        }
        src >> N_order;
        o.resize(N_order);
        for (size_t i = 0; i < N_order; ++i) {
            src >> o[i];
            --o[i];
        }
    }
};
struct GridInfo {
    size_t              N_grid;
    std::vector<size_t> x, y;
    std::vector<int>    pw_actual;
    std::vector<size_t> pw_excess, pw_buy;
    GridInfo() = default;
    GridInfo(size_t N_grid)
        : N_grid(N_grid), x(N_grid), y(N_grid), pw_actual(N_grid), pw_excess(N_grid), pw_buy(N_grid) {}
    void load(std::istream& src, [[maybe_unused]] size_t V = 225, [[maybe_unused]] size_t C_grid_max = 50000) {
        for (size_t i = 0; i < N_grid; ++i) {
            src >> x[i] >> y[i] >> pw_actual[i] >> pw_excess[i] >> pw_buy[i];
            --x[i];
        }
    }
};
std::ostream& operator<<(std::ostream& dest, const GridInfo& i) {
    dest << "\tGrid info:\n";
    for (size_t j = 0; j < i.N_grid; ++j)
        dest << "\t\tx: " << i.x[j] << ", y: " << i.y[j] << ", actual: " << i.pw_actual[j]
             << ", excess: " << i.pw_excess[j] << ", buy: " << i.pw_buy[j] << "\n";
    return dest;
}
struct EvInfo {
    size_t               N_EV;
    std::vector<CarInfo> c;
    EvInfo() = default;
    EvInfo(size_t N_EV) : N_EV(N_EV), c(N_EV) {}
    void load(std::istream& src) {
        for (size_t i = 0; i < N_EV; ++i) {
            c[i].load(src);
        }
    }
};
std::ostream& operator<<(std::ostream& dest, const EvInfo& i) {
    dest << "\tEV info:\n";
    for (size_t j = 0; j < i.N_EV; ++j) dest << "\t\tcar " << j << "\n";
    return dest;
}
struct OrderInfo {
    size_t              N_order;
    std::vector<size_t> id, w, z, state, time;
    OrderInfo() = default;
    void load(std::istream& src, [[maybe_unused]] size_t V = 225, [[maybe_unused]] size_t T_last = 900) {
        src >> N_order;
        id.resize(N_order);
        w.resize(N_order);
        z.resize(N_order);
        state.resize(N_order);
        time.resize(N_order);
        for (size_t i = 0; i < N_order; ++i) {
            src >> id[i] >> w[i] >> z[i] >> state[i] >> time[i];
            --w[i], --z[i];
        }
    }
};
std::ostream& operator<<(std::ostream& dest, const OrderInfo& i) {
    dest << "\tOrder info: " << i.N_order << " orders left\n";
    for (size_t j = 0; j < i.N_order; ++j)
        dest << "\t\tid: " << i.id[j] << ", departure: " << i.w[j] << ", arrival: " << i.z[j]
             << ", state: " << i.state[j] << ", ordered at: " << i.time[j] << "\n";
    return dest;
}
struct GraphSummary {
    vector<vector<size_t>> len;
    vector<vector<size_t>> next;
    vector<size_t>         nano_grid_pos;
    size_t                 diameter     = 0;
    size_t                 cover_radius = 0;
    GraphSummary(const GraphData& graph, const GridData& grid)
        : len(graph.V, std::vector<size_t>(graph.V, 1e9)),
          next(graph.V, std::vector<size_t>(graph.V)),
          nano_grid_pos(grid.N_grid) {
        const size_t V = graph.V;
        for (size_t i = 0; i < V; ++i) len[i][i] = 0;
        for (size_t i = 0; i < V; ++i)
            for (size_t j = 0; j < V; ++j) next[i][j] = j;
        for (const auto& [u, u_edges] : graph.edges)
            for (const auto& [v, length] : u_edges) {
                len[u][v] = length;
                len[v][u] = length;
            }
        for (size_t k = 0; k < V; ++k)
            for (size_t i = 0; i < V; ++i)
                for (size_t j = 0; j < V; ++j)
                    if (len[i][j] > len[i][k] + len[k][j]) {
                        len[i][j]  = len[i][k] + len[k][j];
                        next[i][j] = next[i][k];
                    }
        nano_grid_pos = grid.x;
        for (size_t i = 0; i < V; ++i)
            for (size_t j = 0; j < V; ++j) diameter = max(len[i][j], diameter);
        for (size_t i = 0; i < V; ++i) {
            size_t min_len = 1e9;
            for (size_t j = 0; j < nano_grid_pos.size(); ++j) min_len = min(min_len, len[i][j]);
            cover_radius = max(cover_radius, min_len);
        }
    }
};
size_t transit_length(const std::vector<size_t>& path, const std::vector<std::vector<size_t>>& min_path_len) {
    size_t len = 0;
    for (size_t i = 1; i < path.size(); ++i) len += min_path_len[path[i - 1]][path[i]];
    return len;
}
size_t transit_length(const std::vector<pair<size_t, int>>&   path,
                      const std::vector<std::vector<size_t>>& min_path_len) {
    size_t len = 0;
    for (size_t i = 1; i < path.size(); ++i) len += min_path_len[path[i - 1].first][path[i].first];
    return len;
}
pair<size_t, size_t> nearest_point(size_t current, const vector<size_t>& points, const GraphSummary& gs) {
    size_t len = 1e9, nearest_pos = -1, nearest_index = -1;
    for (size_t i = 0; i < points.size(); ++i)
        if (gs.len[current][points[i]] < len) {
            len           = gs.len[current][points[i]];
            nearest_pos   = points[i];
            nearest_index = i;
        }
    return {nearest_index, nearest_pos};
}

pair<size_t, size_t> nearest_and_stored_energy_point(const GridInfo& gi, const size_t current,
                                                     const vector<size_t>& points, const GraphSummary& gs) {
    size_t   energy = 0, len = 1e9, pos = -1, pos_index = -1;
    uint64_t cost = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        if (gs.len[current][points[i]] < len /*&& gi.pw_actual[i] > Energy*/) {
            // if (1e9 - gs.len[current][points[i]] * gi.pw_actual[i] > cost) {
            cost = (1e9 - gs.len[current][points[i]]) * gi.pw_actual[i];
            len  = gs.len[current][points[i]];
            // Energy    = gi.pw_actual[i];
            pos       = points[i];
            pos_index = i;
        }
    }
    return {pos_index, pos};
}

pair<size_t, size_t> nearest_nano_grid(size_t current, const GraphSummary& gs) {
    return nearest_point(current, gs.nano_grid_pos, gs);
}

pair<size_t, size_t> nearest_and_stored_energy_nano_point(const GridInfo& gi, const size_t current,
                                                          const GraphSummary& gs) {
    return nearest_and_stored_energy_point(gi, current, gs.nano_grid_pos, gs);
}

string path_string(const vector<pair<size_t, int>>& path) {
    string ret;
    for (auto [p, pickup] : path)
        ret += " -> " + to_string(p + 1) + (pickup != -1 ? "(pickup: " + to_string(pickup) + ")" : "");
    return ret;
}
vector<pair<size_t, int>> find_transit_path_greedy(size_t current, const vector<tuple<size_t, size_t, size_t>>& order,
                                                   const GraphSummary& gs) {
    for ([[maybe_unused]] auto [from, to, id] : order) {
    }
    vector<pair<size_t, int>> ret;
    ret.reserve(2 * order.size());
    vector<size_t> pickup_flag(order.size(), 0);
    vector<size_t> index;
    index.reserve(order.size());
    vector<size_t> candidate;
    candidate.reserve(2 * order.size());
    size_t cur = current;
    while (1) {
        for (size_t i = 0; i < order.size(); ++i) switch (pickup_flag[i]) {
                case 0:
                    candidate.push_back(get<0>(order[i]));
                    index.push_back(i);
                    break;
                case 1:
                    candidate.push_back(get<1>(order[i]));
                    index.push_back(i);
                    break;
                default:;
            }
        if (candidate.size() == 0) break;
        for ([[maybe_unused]] auto p : candidate)
            ;
        auto [i, pos] = nearest_point(cur, candidate, gs);
        ret.emplace_back(pos, pickup_flag[index[i]] == 0 ? get<2>(order[index[i]]) : -1);
        pickup_flag[index[i]] += 1;
        cur = pos;
        candidate.clear();
        index.clear();
    }
    return ret;
}
size_t path_length_test(size_t insert_point, size_t insert_index, const std::vector<size_t>& path,
                        const std::vector<std::vector<size_t>>& min_path_len) {
    size_t len = insert_index == 0 ? min_path_len[insert_point][path[0]] : 0;
    for (size_t i = 1; i < path.size(); ++i)
        if (insert_index == i)
            len += min_path_len[path[i - 1]][insert_point] + min_path_len[insert_point][path[i]];
        else
            len += min_path_len[path[i - 1]][path[i]];
    len += insert_index == path.size() ? min_path_len[path.back()][insert_point] : 0;
    return len;
}
struct Action : std::list<std::string> {};
struct MoveEv : Action {
    MoveEv(size_t current, size_t goal, const GraphSummary& gs) {
        for (size_t cur = current; cur != goal; cur = gs.next[cur][goal]) {
            const size_t next = gs.next[cur][goal];
            for (size_t count = 0; count < gs.len[cur][next]; ++count)
                this->push_back("move " + std::to_string(next + 1));
        }
    }
    MoveEv(size_t current, const std::vector<size_t>& path, const GraphSummary& gs) {
        size_t cur = current;
        for (size_t goal : path)
            for (; cur != goal; cur = gs.next[cur][goal]) {
                const size_t next = gs.next[cur][goal];
                for (size_t count = 0; count < gs.len[cur][next]; ++count)
                    this->push_back("move " + std::to_string(next + 1));
            }
    }
};
auto minimal_matching(const vector<size_t>& start, const vector<size_t>& goal, const GraphSummary& gs) {
    auto   minimal_s = start.begin(), minimal_g = goal.begin();
    size_t minimal_len = 1e9;
    for (auto s = start.begin(); s != start.end(); ++s)
        for (auto g = goal.begin(); g != goal.end(); ++g)
            if (gs.len[*s][*g] < minimal_len) {
                minimal_s   = s;
                minimal_g   = g;
                minimal_len = gs.len[*s][*g];
            }
    return make_pair(minimal_s, minimal_g);
}
template <class... Args>
string strprintf(const char* fmt, const Args&... args) {
    char buf[65536];
    sprintf(buf, fmt, args...);
    return buf;
}
template <class P>
struct Strategy : public P {
    const GraphSummary& gs;
    vector<list<string>> command_queue;
    Strategy(const P& p, const GraphSummary& gs) : P(p), gs(gs), command_queue(P::ev_data.N_EV) {}
    virtual void command(const GridInfo& g_i, EvInfo& ev_i, const OrderInfo& order_i) = 0;
    virtual void initialize() {
        for (auto& queue : command_queue) queue.clear();
    }
    bool is_free(size_t EV_index) {
        if (command_queue[EV_index].size() > 0) {
            return false;
        }
        return true;
    }
    string dequeue(const EvInfo& ev_i) {
        string ret = "";
        for (size_t i = 0; i < ev_i.N_EV; ++i) ret += dequeue(i) + "\n";
        return ret;
    }
    string dequeue(size_t EV_index) {
        string ret;
        if (command_queue[EV_index].size() > 0) {
            ret = command_queue[EV_index].front();
            command_queue[EV_index].pop_front();
        } else {
            ret = "stay";
        }
        return ret;
    }
    void enqueue(size_t EV_index, const string& cmd) { command_queue[EV_index].push_back(cmd); }
    void enqueue(size_t EV_index, const string& cmd, size_t repeat) {
        for (size_t i = 0; i < repeat; ++i) command_queue[EV_index].push_back(cmd);
    }
    void enqueue(size_t EV_index, list<string>&& cmd_list) {
        command_queue[EV_index].splice(command_queue[EV_index].end(), cmd_list);
    }
};
struct Energy : Strategy<B> {
    std::set<size_t> assigned_order;
    Energy(const B& b, const GraphSummary& gs) : Strategy<B>(b, gs) {}
    void initialize() override {
        Strategy::initialize();
        assigned_order.clear();
    }
    void update_command(const size_t ev_index, EvInfo& ev_i, const OrderInfo& order_i, const size_t current) {
        std::set<size_t> unassigned_order;
        for (size_t i = 0; i < order_i.N_order; ++i)
            if (assigned_order.count(order_i.id[i]) == 0) unassigned_order.insert(i);
        if (unassigned_order.empty()) return;
        size_t                                     count = 0;
        std::vector<tuple<size_t, size_t, size_t>> assign_order;
        while (!unassigned_order.empty() && count++ < ev_data.N_trans_max) {
            const size_t order_index = *(unassigned_order.begin());
            unassigned_order.erase(unassigned_order.begin());
            const size_t from = order_i.w[order_index];
            const size_t to   = order_i.z[order_index];
            assign_order.emplace_back(from, to, order_i.id[order_index]);
            assigned_order.insert(order_i.id[order_index]);
        }
        auto           path = find_transit_path_greedy(current, assign_order, gs);
        vector<size_t> transit;
        transit.reserve(path.size() + 1);
        const size_t expected_transit_length = transit_length(path, gs.len) + gs.len[current][path[0].first];
        if (ev_i.c[ev_index].charge < (expected_transit_length + gs.cover_radius) * ev_data.Delta_EV_move) {
            size_t need_energy =
                (expected_transit_length + gs.cover_radius) * ev_data.Delta_EV_move - ev_i.c[ev_index].charge;
            enqueue(ev_index, strprintf("charge_from_grid %zu", ev_data.V_EV_max), (need_energy / ev_data.V_EV_max) + 1);
            ev_i.c[ev_index].charge += ev_data.V_EV_max * ((need_energy / ev_data.V_EV_max) + 1);
        }
        size_t cur = current;
        for (auto [to, pick_up] : path) {
#ifdef DEBUG_OUTPUT
            ofs << "ev_i.c[ev_index].charge: " << ev_i.c[ev_index].charge << endl;
            ofs << "gs.len[cur][to]: " << gs.len[cur][to] << " cur: " << cur << " to: " << to << endl;
            ofs << "EV.Delta_EV_move: " << ev_data.Delta_EV_move << endl;
            ofs << "destination: " << path[0].first << endl;
#endif
            enqueue(ev_index, MoveEv(cur, to, gs));
            if (pick_up != -1) enqueue(ev_index, strprintf("pickup %d", pick_up));
            cur = to;
#ifdef DEBUG_OUTPUT
            ofs << "---------------------" << endl;
#endif
        }
    }
    void command(const GridInfo& gi, EvInfo& ev_info, const OrderInfo& order_i) override {
        for (size_t ev_index = 0; ev_index < ev_info.N_EV; ++ev_index) {
            if (!is_free(ev_index)) continue;
            const size_t current       = ev_info.c[ev_index].u;
            const size_t safety_energy = ev_data.Delta_EV_move * 60;
            if (auto [_, pos] = nearest_and_stored_energy_nano_point(gi, current, gs); current != pos) {
                const size_t len_to_charge   = gs.len[current][pos];
                const int    expected_energy = ev_info.c[ev_index].charge - len_to_charge * ev_data.Delta_EV_move;
                if (expected_energy < 0) {
                    enqueue(ev_index, "stay", 1000);
                } else
                    enqueue(ev_index, MoveEv(current, pos, gs));
                ev_info.c[ev_index].charge -= expected_energy;
                continue;
            } else {
                if (ev_info.c[ev_index].charge < safety_energy) {
                    enqueue(ev_index,
                            strprintf("charge_from_grid %zu", ev_data.V_EV_max),
                            ceil(1.0 * (safety_energy - ev_info.c[ev_index].charge) / ev_data.V_EV_max));
                    ev_info.c[ev_index].charge = ev_data.V_EV_max;
                    continue;
                }
            }
            update_command(ev_index, ev_info, order_i, current);
        }
    }
};
template <class P>
struct all_stay : Strategy<P> {
    all_stay(const P& p, const GraphSummary& gs) : Strategy<P>(p, gs) {}
    void command(const GridInfo&, const EvInfo&, const OrderInfo&) {}
};
template <class P>
struct random_walk : Strategy<P> {
    using S = Strategy<P>;
    std::mt19937_64 engine;
    random_walk(const P& p, const GraphSummary& gs) : Strategy<P>(p, gs) {}
    void command(const GridInfo&, EvInfo& ev_i, const OrderInfo&) {
        for (size_t n = 0; n < ev_i.N_EV; ++n) {
            if (!S::is_free(n)) continue;
            const size_t current       = ev_i.c[n].u;
            const size_t safety_energy = S::EV.Delta_EV_move * 50;
            if (auto [_, pos] = nearest_nano_grid(current, S::gs); current != pos) {
                const size_t len_to_charge   = S::gs.len[current][pos];
                const int    expected_energy = ev_i.c[n].charge - len_to_charge * S::EV.Delta_EV_move;
                if (expected_energy < 0) {
                    S::enqueue(n, "stay", 1000);
                } else
                    S::enqueue(n, MoveEv(current, pos, S::gs));
                continue;
            } else {
                if (ev_i.c[n].charge < safety_energy) {
                    S::enqueue(n,
                               strprintf("charge_from_grid %zu", S::EV.V_EV_max),
                               ceil(1.0 * (safety_energy - ev_i.c[n].charge) / S::EV.V_EV_max));
                    continue;
                }
            }
            uniform_int_distribution<size_t> dice(0, ev_i.c[n].N_adj - 1);
            const size_t                     goal = dice(engine);
            S::enqueue(n, MoveEv(current, ev_i.c[n].a[goal], S::gs));
        }
    }
};
struct transport_only_0 : Strategy<B> {
    std::set<size_t> assigned_order;
    transport_only_0(const B& b, const GraphSummary& gs) : Strategy<B>(b, gs) {}
    void initialize() override {
        Strategy::initialize();
        assigned_order.clear();
    }
    void command(const GridInfo&, EvInfo& ev_i, const OrderInfo& order_i) override {
        for (size_t n = 0; n < ev_i.N_EV; ++n) {
            if (!is_free(n)) continue;
            const size_t current       = ev_i.c[n].u;
            const size_t safety_energy = ev_data.Delta_EV_move * 50;
            if (auto [_, pos] = nearest_nano_grid(current, gs); current != pos) {
                const size_t len_to_charge   = gs.len[current][pos];
                const int    expected_energy = ev_i.c[n].charge - len_to_charge * ev_data.Delta_EV_move;
                if (expected_energy < 0) {
                    enqueue(n, "stay", 1000);
                } else
                    enqueue(n, MoveEv(current, pos, gs));
                continue;
            } else {
                if (ev_i.c[n].charge < safety_energy) {
                    enqueue(n,
                            strprintf("charge_from_grid %zu", ev_data.V_EV_max),
                            ceil(1.0 * (safety_energy - ev_i.c[n].charge) / ev_data.V_EV_max));
                    continue;
                }
            }
            std::set<size_t> unassigned_order;
            for (size_t i = 0; i < order_i.N_order; ++i)
                if (assigned_order.count(order_i.id[i]) == 0) unassigned_order.insert(i);
            if (!unassigned_order.empty()) {
                size_t                                     count = 0;
                std::vector<tuple<size_t, size_t, size_t>> assign_order;
                while (!unassigned_order.empty() && count++ < ev_data.N_trans_max) {
                    const size_t order_index = *(unassigned_order.begin());
                    unassigned_order.erase(unassigned_order.begin());
                    const size_t from = order_i.w[order_index];
                    const size_t to   = order_i.z[order_index];
                    assign_order.emplace_back(from, to, order_i.id[order_index]);
                    assigned_order.insert(order_i.id[order_index]);
                }
                auto           path = find_transit_path_greedy(current, assign_order, gs);
                vector<size_t> transit;
                transit.reserve(path.size() + 1);
                const size_t expected_transit_length = transit_length(path, gs.len) + gs.len[current][path[0].first];
                if (ev_i.c[n].charge < (expected_transit_length + gs.cover_radius) * ev_data.Delta_EV_move) {
                    enqueue(n,
                            strprintf("charge_from_grid %zu", ev_data.V_EV_max),
                            ((expected_transit_length + gs.cover_radius) * ev_data.Delta_EV_move - ev_i.c[n].charge) /
                                    ev_data.V_EV_max +
                                1);
                }
                size_t cur = current;
                for (auto [to, pick_up] : path) {
                    enqueue(n, MoveEv(cur, to, gs));
                    if (pick_up != -1) enqueue(n, strprintf("pickup %d", pick_up));
                    cur = to;
                }
                continue;
            } else {
            }
            continue;
        }
    }
};
vector<string> split_command(const string& command_pack) {
    vector<string> ret;
    stringstream   reader(command_pack);
    string         line;
    while (getline(reader, line)) {
        if (line == "")
            continue;
        else
            ret.emplace_back(line);
    }
    return ret;
}
enum command_type { stay, move, pickup, charge_from_grid, charge_to_grid, invalid_command };
struct command {
    command_type type;
    size_t       val;
    command(command_type type, size_t val) : type(type), val(val) {}
    string to_str() const {
        switch (type) {
            case command_type::stay:
                return strprintf("stay");
            case command_type::move:
                return strprintf("move %zu", val);
            case command_type::pickup:
                return strprintf("pickup %zu", val);
            case command_type::charge_from_grid:
                return strprintf("charge_from_grid %zu", val);
            case command_type::charge_to_grid:
                return strprintf("charge_to_grid %zu", val);
            default:
                break;
        }
        return "";
    }
};
command parser(const string& command) {
    stringstream reader(command);
    string       command_str;
    size_t       value;
    reader >> command_str >> value;
    if (command_str == "stay") {
        return {command_type::stay, 0};
    } else if (command_str == "move") {
        return {command_type::move, value};
    } else if (command_str == "pickup") {
        return {command_type::pickup, value};
    } else if (command_str == "charge_from_grid") {
        return {command_type::charge_from_grid, value};
    } else if (command_str == "charge_to_grid") {
        return {command_type::charge_to_grid, value};
    }
    return {invalid_command, (size_t)-1};
}

class BHokudaiHitachi2020B {
 public:
    void solve(std::istream& cin, std::ostream& cout) {
        setbuf(log_dest, nullptr);
        size_t N_solution = 1;
        cin >> N_solution;
        B                            prob(cin);
        std::shared_ptr<Strategy<B>> str = nullptr;
        GraphSummary                 gs(prob.graph_data, prob.grid_data);
        GridInfo                     grid_i(prob.grid_data.N_grid);
        EvInfo                       ev_i(prob.ev_data.N_EV);
        OrderInfo                    order_i;
        string                       command_per_turn;
        vector<pair<double, double>> scores;
        scores.reserve(N_solution);
        for (size_t n = 0; n < N_solution; ++n) {
            // str.reset(new all_stay<B>(prob, gs));
            // str.reset(new random_walk<B>(prob, gs));
            // str = std::make_shared<transport_only_0>(prob, gs);
            str = std::make_shared<Energy>(prob, gs);
            str->initialize();
            for (size_t t = 0; t < prob.T_max; ++t) {
                grid_i.load(cin);
                ev_i.load(cin);
                order_i.load(cin);
                str->command(grid_i, ev_i, order_i);
                command_per_turn  = str->dequeue(ev_i);
                auto command_list = split_command(command_per_turn);
                cout << command_per_turn << flush;
            }
            grid_i.load(cin);
            ev_i.load(cin);
            order_i.load(cin);
            double S_trans, S_ele;
            cin >> S_trans >> S_ele;
        }
    }
};
