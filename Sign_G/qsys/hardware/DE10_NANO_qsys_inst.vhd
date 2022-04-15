	component DE10_NANO_qsys is
		port (
			adc_ltc2308_CONVST     : out std_logic;        -- CONVST
			adc_ltc2308_SCK        : out std_logic;        -- SCK
			adc_ltc2308_SDI        : out std_logic;        -- SDI
			adc_ltc2308_SDO        : in  std_logic := 'X'; -- SDO
			clk_clk                : in  std_logic := 'X'; -- clk
			pll_qsys_locked_export : out std_logic;        -- export
			reset_reset_n          : in  std_logic := 'X'; -- reset_n
			uart_0_rxd             : in  std_logic := 'X'; -- rxd
			uart_0_txd             : out std_logic;        -- txd
			uart_1_rxd             : in  std_logic := 'X'; -- rxd
			uart_1_txd             : out std_logic         -- txd
		);
	end component DE10_NANO_qsys;

	u0 : component DE10_NANO_qsys
		port map (
			adc_ltc2308_CONVST     => CONNECTED_TO_adc_ltc2308_CONVST,     --     adc_ltc2308.CONVST
			adc_ltc2308_SCK        => CONNECTED_TO_adc_ltc2308_SCK,        --                .SCK
			adc_ltc2308_SDI        => CONNECTED_TO_adc_ltc2308_SDI,        --                .SDI
			adc_ltc2308_SDO        => CONNECTED_TO_adc_ltc2308_SDO,        --                .SDO
			clk_clk                => CONNECTED_TO_clk_clk,                --             clk.clk
			pll_qsys_locked_export => CONNECTED_TO_pll_qsys_locked_export, -- pll_qsys_locked.export
			reset_reset_n          => CONNECTED_TO_reset_reset_n,          --           reset.reset_n
			uart_0_rxd             => CONNECTED_TO_uart_0_rxd,             --          uart_0.rxd
			uart_0_txd             => CONNECTED_TO_uart_0_txd,             --                .txd
			uart_1_rxd             => CONNECTED_TO_uart_1_rxd,             --          uart_1.rxd
			uart_1_txd             => CONNECTED_TO_uart_1_txd              --                .txd
		);

