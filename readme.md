# Wiener Simulation

A monte Carlo option pricer written in C++, using the exact Geometric Brownian Motion equations and antithetic variates for variance reduciton.
It prices both European call and put options, and validates its results against the solution obtained through the Black-Scholes equations.

## What it does
 - Simulates n possible future stock prices under GBM
 - Prices the European call and put options by averaging discounted payoffs across all simulated paths
 - It then compares the results against the Black-Scholes closed-form formula, and gives relative error
 - Is able to plot simulated stock price paths to establish visual correctnes

## Usage
To build, run, and plot, you can use the provided script:
```simulateAndPlot.sh```

To use the pricing functionality use: 
```./WienerSimulation <mode> flags```

Example output with default argument values:
```
Black-Scholes Call Price: 10.4506
Monte Carlo Call Price: 10.4192
Relative error: 0.300541%

Black-Scholes Put Price: 5.57353
Monte Carlo Put Price: 5.55855
Relative error: 0.268669%
```

### All flags

| Flag | Description | Default |
|------|-------------|---------|
| `--spot` | Current stock price | 100 |
| `--strike` | Strike price | 100 |
| `--rate` | Risk-free interest rate (annualised) | 0.05 |
| `--vol` | Volatility (annualised) | 0.20 |
| `--expiry` | Time to expiry in years | 1.0 |
| `--paths` | Number of Monte Carlo paths | 1000 |


## Why is Euler-Maryama code in here?

The Euler-Maruyama discretisation implementation is kept as it was my original implementation, before transitioning to using a closed-form solution. 
As GBM is one of the few SDEs with a closed-form solution, the pricer is able to use exact sampling. 
Other moodels like jump-diffusion don't have clean closed-forms solutions, so Euler-Maruyama is the general method you'd fall back to for those.