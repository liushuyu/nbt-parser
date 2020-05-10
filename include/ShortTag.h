//
// Created by cth451 on 2020/05/09.
//

#ifndef NBTP_SHORTTAG_H
#define NBTP_SHORTTAG_H

#include "Tag.h"

namespace NBTP {
	class ShortTag : public Tag {
	public:
		typedef int16_t V;
	private:
		V payload;
	public:
		/**
		 * @return NBT type code compound 1
		 */
		static int8_t typeCode() noexcept;
		std::ostream &output(std::ostream &ostream, IOFormat format) override;
		std::ostream &textOutput(std::ostream &ostream, unsigned int indent) override;
		ShortTag(V value);
		/**
		 * Deserializer constructor
		 * @param input stream to read a single byte
		 */
		ShortTag(std::istream& input);
	};
}

#endif //NBTP_BYTETAG_H
