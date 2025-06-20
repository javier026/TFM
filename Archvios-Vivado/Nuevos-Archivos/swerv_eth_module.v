`timescale 1 ns / 1 ps

module swerv_eth
(
	input  wire           clk_100,
	input  wire           cpu_rst_n,
	
	output wire           eth_mdc,
	inout  wire           eth_mdio,
	output wire           eth_rstn,
	inout  wire           eth_crsdv,
	inout  wire           eth_rxerr,
	inout  wire [1:0]      eth_rxd,
	output wire           eth_txen,
	output wire [1:0]      eth_txd,
	output wire           eth_clkin,
	inout  wire           eth_intn,
	input  wire           btnu,
	//ethernet wishbone
	input  wire [31:0] wb_eth_adr_i,
    input wire [31:0] wb_eth_dat_i,
    input wire  [3:0] wb_eth_sel_i,
    input wire        wb_eth_we_i,
    input  wire        wb_eth_stb_i,
    input wire  [2:0] wb_eth_cti_i,
    input wire  [1:0] wb_eth_bte_i,
    input wire        wb_eth_cyc_i,
    output  wire [31:0] wb_eth_dat_o,
    output  wire         wb_eth_ack_o,
    output  wire        wb_eth_err_o,
    output  wire        wb_eth_rty_o
);

	integer i;
	
	wire clk_mac;
	wire clk_phy;
	wire clk_fb;
	wire pll_locked;
	PLLE2_BASE#
	(
		.CLKFBOUT_MULT (10),
		.CLKOUT0_DIVIDE(20),
		.CLKOUT1_DIVIDE(20),
		.CLKOUT1_PHASE (45.0),
		.CLKIN1_PERIOD (10.0)
	)
	clk_gen 
	(
		.CLKOUT0 (clk_mac),
		.CLKOUT1 (clk_phy),
		.CLKFBOUT(clk_fb),
		.LOCKED  (pll_locked),
		.CLKIN1  (clk_100),
		.RST     (1'b0),
		.CLKFBIN (clk_fb)
	);
	
	reg        rst_n         = 0;
	reg [15:0] rst_n_counter = 0;
	always @(posedge clk_mac) begin
		rst_n         <= (rst_n || &rst_n_counter) && pll_locked && cpu_rst_n;
		rst_n_counter <= rst_n ? 0 : rst_n_counter + 1;
	end
	
	wire btnu_d;
	debounce#(1) btnu_debounce
	(
		.clk(clk_mac),
		.in (btnu),
		.out(btnu_d)
	);
	
	(* mark_debug = "true" *)
	wire [7:0]  rx_axis_mac_tdata;
	(* mark_debug = "true" *)
	wire        rx_axis_mac_tvalid;
	(* mark_debug = "true" *)
	wire        rx_axis_mac_tlast;
	(* mark_debug = "true" *)
	wire        rx_axis_mac_tuser;
	(* mark_debug = "true" *)
	reg [7:0]   tx_axis_mac_tdata;
	(* mark_debug = "true" *)
	reg         tx_axis_mac_tvalid;
	(* mark_debug = "true" *)
	reg         tx_axis_mac_tlast;
	(* mark_debug = "true" *)
	wire        tx_axis_mac_tready;
	reg         reg_vld = 0;
	reg  [4:0]  reg_addr;
	reg         reg_write;
	reg  [15:0] reg_wval;
	wire [15:0] reg_rval;
	wire        reg_ack;
	eth_mac#(1) mac_inst
	(
		.clk_mac    (clk_mac),
		.clk_phy    (clk_phy),
		.rst_n      (rst_n),
		.mode_straps(3'b111),
	
		.eth_mdc  (eth_mdc),
		.eth_mdio (eth_mdio),
		.eth_rstn (eth_rstn),
		.eth_crsdv(eth_crsdv),
		.eth_rxerr(eth_rxerr),
		.eth_rxd  (eth_rxd),
		.eth_txen (eth_txen),
		.eth_txd  (eth_txd),
		.eth_clkin(eth_clkin),
		.eth_intn (eth_intn),
		
		.rx_axis_mac_tdata (rx_axis_mac_tdata),
		.rx_axis_mac_tvalid(rx_axis_mac_tvalid),
		.rx_axis_mac_tlast (rx_axis_mac_tlast),
		.rx_axis_mac_tuser (rx_axis_mac_tuser),
		.tx_axis_mac_tdata (tx_axis_mac_tdata),
		.tx_axis_mac_tvalid(tx_axis_mac_tvalid),
		.tx_axis_mac_tlast (tx_axis_mac_tlast),
		.tx_axis_mac_tready(tx_axis_mac_tready),
		
		.reg_vld  (reg_vld),
		.reg_addr (reg_addr),
		.reg_write(reg_write),
		.reg_wval (reg_wval),
		.reg_rval (reg_rval),
		.reg_ack  (reg_ack)
	);
	
	reg [7:0] tx_pkt [63:0];
	reg [7:0] tx_idx = 65;
	initial begin
	    //MAC destino (broadcast en este caso)
		tx_pkt[0]  = 8'hff;
		tx_pkt[1]  = 8'hff;
		tx_pkt[2]  = 8'hff;
		tx_pkt[3]  = 8'hff;
		tx_pkt[4]  = 8'hff;
		tx_pkt[5]  = 8'hff;
		//MAC origen
		tx_pkt[6]  = 8'h0;
		tx_pkt[7]  = 8'h0;
		tx_pkt[8]  = 8'h0;
		tx_pkt[9]  = 8'h0;
		tx_pkt[10] = 8'h0;
		tx_pkt[11] = 8'h0;
		//Tipo de ethernet
		tx_pkt[12] = 8'heb;
		tx_pkt[13] = 8'heb;
		//Payload
		//for(i = 14; i < 64; i = i + 1)
			//tx_pkt[i] = i;
		tx_pkt[14] = 8'h44;  // D
        tx_pkt[15] = 8'h61;  // a
        tx_pkt[16] = 8'h74;  // t
        tx_pkt[17] = 8'h6F;  // o
        tx_pkt[18] = 8'h20;  // espacio
        //tx_pkt[19] = 8'h31;  // 1
        //tx_pkt[20] = 8'h30;  // 0
        
	end
	
	//////////////////////
	
	wire env_sel;
	wire dat_sel;
	wire ack;
    assign env_sel = wb_eth_cyc_i & wb_eth_stb_i & (wb_eth_adr_i[5:2] == 4'h0); //0x80001800
    assign dat_sel = wb_eth_cyc_i & wb_eth_stb_i & (wb_eth_adr_i[5:2] == 4'h1); //0x80001804
   
    assign ack = wb_eth_cyc_i & wb_eth_stb_i;
   
   
    always @(posedge clk_mac or posedge cpu_rst_n)
        if (cpu_rst_n)
            wb_eth_ack_o <= #1 1'b0;
        else
            wb_eth_ack_o <= #1 ack & ~wb_eth_ack_o ;
   
    reg [15:0] data_env;
    
    //always @(posedge clk_mac or posedge cpu_rst_n)
      //  if (cpu_rst_n)
        //    data_env <= #1 12'b0;
       // else if (dat_sel)
         //   data_env <= #1 wb_eth_dat_i[15:0];
	
	//////////////////////
	
	always @(posedge clk_mac) begin
		if(btnu_d || env_sel) begin //(env_sel)
			tx_idx             <= 0;
			tx_axis_mac_tdata  <= tx_pkt[0];
			tx_axis_mac_tvalid <= 1;
			tx_axis_mac_tlast  <= 0;
		end else if(tx_idx < 21) begin // Envia mientras queden bytes por enviar (14 de cabecera + x de datos)
			tx_axis_mac_tvalid <= 1;
			tx_axis_mac_tdata  <= tx_pkt[tx_idx + tx_axis_mac_tready];
			tx_axis_mac_tlast  <= (tx_idx + tx_axis_mac_tready) == 20; //Indice final
			tx_idx             <= tx_idx + tx_axis_mac_tready;
		end else
			tx_axis_mac_tvalid <= 0;
	end
	
	localparam STATE_RST       = 0;
	localparam STATE_IDLE      = 1;
	localparam STATE_CHECK_REG = 2;
	
	reg [2:0]  state, next_state;
	reg [20:0] count = 0;
	always @(posedge clk_mac) begin
		state <= rst_n ? next_state : STATE_RST;
		//led   <= next_led;
		count <= count + 1;
	end
	
	always @* begin
		next_state = state;       
		//next_led   = led;
		reg_vld    = 0;
		reg_write  = 0;
		reg_addr   = 0;
		reg_wval   = 0;
		
		case(state)
			STATE_RST: begin
				next_state = STATE_IDLE;
			end STATE_IDLE: begin
				if(&count)
					next_state = STATE_CHECK_REG;
			end STATE_CHECK_REG: begin
				reg_vld  = 1;
				if(reg_ack) begin
					next_state = STATE_IDLE;
					//next_led   = reg_rval;
				end
			end
		endcase
	end
	
endmodule
