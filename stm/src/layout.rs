type A = keyberon::action::Action<core::convert::Infallible>;
use keyberon::{action::{k, m}, key_code::{KbHidReport, KeyCode::*}, layout::Layers};
use crate::app::Encoders;

macro_rules! s { ($k:expr) => { m(&$k.as_slice()) } }


const NO: A = k(No);

const OTB1: A = s!([LGui, LCtrl, LAlt, V]); // VM
const OTB2: A = s!([LGui, LCtrl, LShift, LAlt, L]); // LOCK

const TB1_UP: A = s!([No]);
const TB1_DOWN: A = s!([LGui, LCtrl, LShift, LAlt, V]); //VM SHUTDOWN

const TB2_UP: A = s!([LGui, LCtrl, A]); // SWITCH RGB PROFILES
const TB2_DOWN: A = s!([LGui, LCtrl, LShift, A]); // START OPENRGB SERVER

const TB3_UP: A = s!([LGui, LShift, F12]); // SWITCH OUTPUT DEVICE
const TB3_DOWN: A = s!([LGui, LShift, F12]); // SWITCH OUTPUT DEVICE

const BLK1: A = s!([LGui, W]);
const BLK2: A = s!([No]);
const BLK3: A = s!([No]);
const BLK4: A = s!([LGui, LCtrl, LAlt, A]); // STOP RGB
const BLK5: A = s!([No]);

const RB_T: A = s!([LGui, F8]); // MUTE MIC
const RB_B: A = s!([LGui, S]); // EWW BOARD

const SILV1: A = s!([MediaPlayPause]);
const SILV2: A = s!([MediaPreviousSong]);
const SILV3: A = s!([MediaNextSong]);

const ENC_1: A = s!([LGui, LShift, LAlt, F7]); // MUTE MUSIC
const ENC_2: A = s!([LGui, LShift, LAlt, F8]); // MUTE BROWSER

pub const LAYERS: Layers<8, 3, 1> = [
    [
        [  OTB1,   OTB2,  TB1_UP, TB2_UP, TB3_UP, TB3_DOWN, TB2_DOWN, TB1_DOWN ],
        [  BLK1,   BLK2,    BLK3,   BLK4,   BLK5,     RB_T,       NO,       NO ],
        [ SILV1,  SILV2,   SILV3,  ENC_1,  ENC_2,     RB_B,       NO,       NO ]
    ]
];




pub fn check_encoders(kr: &mut KbHidReport, encoders: &mut Encoders) {
    let enc_pin_states: [bool; 4] =
        [
            encoders.enc_1.0.is_low(),
            encoders.enc_1.1.is_low(),
            encoders.enc_2.0.is_low(),
            encoders.enc_2.1.is_low()
        ];

    let enc_last_state = &mut encoders.enc_last_state;
    let enc_block = &mut encoders.enc_block;

    (0..4).for_each(|i| {
        let state_has_change = enc_last_state[i] != enc_pin_states[i];

        if 
            state_has_change &&
            enc_pin_states[i] &&
            !enc_block[i] 
        {
            kr.pressed(LGui);
            kr.pressed(LShift);
            kr.pressed(LAlt);
            match i {
                0 => kr.pressed(F9),
                1 => kr.pressed(F10),
                2 => kr.pressed(F12),
                3 => kr.pressed(F11),
                _ => ()
            }

            enc_block[i + (i % 2 == 0) as usize * 2 - 1] = true
        }

        else if 
            state_has_change &&
            enc_block.contains(&false) 
        {
            enc_block.fill(false)
        }
    });

    *enc_last_state = enc_pin_states;
}
