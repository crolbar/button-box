#![no_std]
#![no_main]

use panic_semihosting as _;
use stm32f1xx_hal as hal;

mod layout;

#[rtic::app(device = hal::pac, peripherals = true)]
mod app {
    use super::*;

    use layout::{LAYERS, check_encoders};
    use usb_device::bus::UsbBusAllocator;
    use keyberon::{
        debounce::Debouncer,
        key_code::KbHidReport,
        layout::{Event, Layout},
        matrix::Matrix
    };

    use hal::{
        gpio::{EPin ,Input, Output, PullUp, PushPull},
        pac, prelude::*, timer,
        usb::{Peripheral, UsbBus, UsbBusType}
    };

    pub struct Leds {}
    impl keyberon::keyboard::Leds for Leds {}

    pub struct Encoders {
        pub enc_1: (EPin<Input<PullUp>>, EPin<Input<PullUp>>),
        pub enc_2: (EPin<Input<PullUp>>, EPin<Input<PullUp>>),
        pub enc_last_state: [bool; 4],
        pub enc_block: [bool; 4],
    }

    #[shared]
    struct Shared {
        usb_dev: usb_device::device::UsbDevice<'static, UsbBusType>,
        usb_class: keyberon::Class<'static, UsbBusType, Leds>,
    }

    #[local]
    struct Local {
        layout: Layout<8, 3, 1>,
        matrix: Matrix<EPin<Input<PullUp>>, EPin<Output<PushPull>>, 8, 3>,
        debouncer: Debouncer<[[bool; 8]; 3]>,
        timer: timer::CounterHz<pac::TIM3>,
        encoders: Encoders,
    }

    #[init(local = [bus: Option<UsbBusAllocator<UsbBusType>> = None])]
    fn init(mut ctx: init::Context) -> (Shared, Local, init::Monotonics) {
        let rcc = ctx.device.RCC.constrain();

        // dfu on reset
        rcc.bkp.constrain(ctx.device.BKP, &mut ctx.device.PWR)
            .write_data_register_low(9, 0x424C);

        let clocks = rcc.cfgr
            .use_hse(8.MHz())
            .sysclk(72.MHz())
            .pclk1(36.MHz())
            .freeze(
                &mut ctx.device.FLASH.constrain().acr
            );

        let mut gpioa = ctx.device.GPIOA.split();
        let mut gpiob = ctx.device.GPIOB.split();
        let mut gpioc = ctx.device.GPIOC.split();

        // force reset to load firmware
        let mut pin_dp = gpioa.pa12.into_push_pull_output(&mut gpioa.crh);
        pin_dp.set_low();
        cortex_m::asm::delay(clocks.sysclk().raw() / 100);


        // turn off onboard led
        gpioc.pc13.into_push_pull_output(&mut gpioc.crh).set_high();



        *ctx.local.bus = Some(
            UsbBus::new(Peripheral {
                usb: ctx.device.USB,
                pin_dm: gpioa.pa11,
                pin_dp: pin_dp.into_floating_input(&mut gpioa.crh),
            })
        );
        let usb_bus = ctx.local.bus.as_ref().unwrap();


        
        let usb_class = keyberon::new_class(usb_bus, Leds{});
        let usb_dev = keyberon::new_device(usb_bus);



        let mut timer = ctx.device.TIM3.counter_hz(&clocks);
        timer.start(1.kHz()).unwrap();
        timer.listen(timer::Event::Update);


        let mut afio = ctx.device.AFIO.constrain();
        let (gpioa_pa15, gpiob_pb3, gpiob_pb4) =
            afio.mapr.disable_jtag(gpioa.pa15, gpiob.pb3, gpiob.pb4);

        let matrix = Matrix::new(
            [ // columns
                gpioa_pa15.into_pull_up_input(&mut gpioa.crh).erase(),
                gpiob_pb3.into_pull_up_input(&mut gpiob.crl).erase(),
                gpiob_pb4.into_pull_up_input(&mut gpiob.crl).erase(),
                gpiob.pb5.into_pull_up_input(&mut gpiob.crl).erase(),
                gpiob.pb6.into_pull_up_input(&mut gpiob.crl).erase(),
                gpiob.pb7.into_pull_up_input(&mut gpiob.crl).erase(),
                gpiob.pb8.into_pull_up_input(&mut gpiob.crh).erase(),
                gpiob.pb9.into_pull_up_input(&mut gpiob.crh).erase(),
            ],
            [ // rows
                gpioa.pa1.into_push_pull_output(&mut gpioa.crl).erase(),
                gpioa.pa2.into_push_pull_output(&mut gpioa.crl).erase(),
                gpioa.pa3.into_push_pull_output(&mut gpioa.crl).erase(),
            ],
        ).unwrap();
        



        let layout = Layout::new(&LAYERS);
        let debouncer = Debouncer::new([[false; 8]; 3], [[false; 8]; 3], 5);
        let encoders = Encoders {
            enc_1: (
                gpiob.pb15.into_pull_up_input(&mut gpiob.crh).erase(),
                gpioa.pa8.into_pull_up_input(&mut gpioa.crh).erase()
            ),
            enc_2: (
                gpioa.pa9.into_pull_up_input(&mut gpioa.crh).erase(),
                gpioa.pa10.into_pull_up_input(&mut gpioa.crh).erase()
            ),
            enc_last_state: [false; 4],
            enc_block: [false; 4],
        };

        (
            Shared { usb_dev, usb_class },
            Local {
                layout,
                debouncer,
                matrix,
                timer,
                encoders,
            },
            init::Monotonics()
        )
    }


    #[task(binds = TIM3, priority = 1, local = [matrix, debouncer, timer, layout, encoders], shared = [usb_class, usb_dev])]
    fn tick(mut ctx: tick::Context) {
        ctx.local.timer.clear_interrupt(timer::Event::Update);

        // release all pressed buttons
        for (ir, r) in ctx.local.debouncer.get().iter().enumerate() {
            for (is, s) in r.iter().enumerate() {
                if *s {
                    ctx.local.layout.event(Event::Release(ir as u8, is as u8));
                }
            }
        }

        for e in ctx.local
            .debouncer
            .events(
                ctx.local.matrix.get().unwrap()
            ) 
        {
            ctx.local.layout.event(e);
        }
        ctx.local.layout.tick();

        let mut report = KbHidReport::from_iter(ctx.local.layout.keycodes());

        check_encoders(&mut report, ctx.local.encoders);

        if ctx.shared.usb_class.lock(|kb| kb.device_mut().set_keyboard_report(report.clone())) {
            while let Ok(0) = ctx.shared.usb_class.lock(|kb| kb.write(report.as_bytes())) {}
        }
    }

    use usb_device::class::UsbClass as _;

    #[task(binds = USB_HP_CAN_TX, priority = 2, shared = [usb_dev, usb_class])]
    fn usb_tx(ctx: usb_tx::Context) {
        (ctx.shared.usb_dev, ctx.shared.usb_class)
        .lock(|usb_dev, kb| {
            if usb_dev.poll(&mut [kb]) {
                kb.poll()
            }
        })
    }

    #[task(binds = USB_LP_CAN_RX0, priority = 2, shared = [usb_dev, usb_class])]
    fn usb_rx(ctx: usb_rx::Context) {
        (ctx.shared.usb_dev, ctx.shared.usb_class)
        .lock(|usb_dev, kb| {
            if usb_dev.poll(&mut [kb]) {
                kb.poll()
            }
        })
    }
}
