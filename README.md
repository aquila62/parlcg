# parlcg
Linear congruential random number generator (LCG) designed to run in
parallel on a supercomputer.  This is a demonstration of how to run
the elcg random number generator in parallel.  Each node on a supercomputer
can run one instance of the RNG using a unique password as its initial
seed.  This illustrates how to do Monte Carlo simulation in parallel.
The example given here is elcgdots, which paints random dots on an
X Windows screen.  As a visual test, you can observe the instances
running in parallel.
