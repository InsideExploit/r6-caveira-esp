namespace Config {

	namespace Offsets {

		constexpr auto pGameManager = 0x5D8E1B8;
		constexpr auto pRoundManager = 0x71140D8;
		constexpr auto pVTable = 0x5326930;

		namespace EntityList {
			constexpr auto pChain = 0x108;
			constexpr auto pObject = 0x8;
			constexpr auto pPawn = 0x50;

			namespace Info {
				constexpr auto pChain_1 = 0x18;
				constexpr auto pChain_2 = 0xD8;

			}

			namespace Entity {
				constexpr auto pChain_1 = 0x80;
				constexpr auto pChain_2 = 0xF0;

			}

			namespace Marker {
				constexpr auto pSpotted = 0x69A;
			}

			namespace State {
				constexpr auto pChain_1 = 0x300;
			}

		}

	}

	namespace Decryption {

		namespace EntityList {
			constexpr auto pDecryption1 = 0xEC2581B1EC5CC398;
			constexpr auto pDecryption2 = 0x71B65F65B607DE6F;
			constexpr auto pDecryption3 = 0xC6554B199E494084;

		}
	}

}