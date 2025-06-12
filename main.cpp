#include "solver.hpp"
#include "physics.hpp"
#include "io.hpp"

#include <filesystem>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>

static std::string prepare_output_dir(){
    namespace fs = std::filesystem;
    fs::path base("Result");
    if(fs::exists(base) && !fs::is_empty(base)){
        auto ts = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        fs::rename(base, "Result_"+std::to_string(ts));
    }
    fs::create_directory(base);
    return "Result";
}

int main(){
    const int nx=64, ny=64;
    const double Lx=1.0, Ly=1.0;
    const double dx=Lx/(nx-1), dy=Ly/(ny-1);
    const int max_steps=200;
    const int output_every=20;

    std::string out_dir = prepare_output_dir();

    FlowField flow(nx,ny,dx,dy);
    // Use peak Bx initial condition from the divergence cleaning paper
    initialize_peak_bx(flow);

    auto t0=std::chrono::high_resolution_clock::now();
    for(int step=0; step<=max_steps; ++step){
        double dt = compute_cfl_timestep(flow);

        divergence_cleaning_step(flow, dt);

        if(step % output_every == 0){
            std::cout << "step " << std::setw(4) << step
                      << " dt=" << dt << "\n";
            save_flow_MHD(flow, out_dir, step);
        }
    }
    auto t1=std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = t1 - t0;
    std::cout << "Total time " << elapsed.count() << " s\n";
    return 0;
}
