//
// Created by cth451 on 2020/05/13.
//

#ifndef NBTP_PYBYTESBUF_H
#define NBTP_PYBYTESBUF_H

#include <ostream>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/cast.h>

namespace pyNBTP {
	/**
	 * This is a modified version of pythonbuf from pybind11 to read / write bytes
	 */
	class PyOBytesBuf : public std::streambuf {
	private:
		using traits_type = std::streambuf::traits_type;

		const size_t buf_size;
		std::unique_ptr<char[]> d_buffer;
		pybind11::object pywrite;
		pybind11::object pyflush;

		int overflow(int c) {
			if (!traits_type::eq_int_type(c, traits_type::eof())) {
				*pptr() = traits_type::to_char_type(c);
				pbump(1);
			}
			return sync() == 0 ? traits_type::not_eof(c) : traits_type::eof();
		}

		int sync() {
			if (pbase() != pptr()) {
				pybind11::bytes line(pbase(), static_cast<size_t>(pptr() - pbase()));

				{
					pybind11::gil_scoped_acquire tmp;
					pywrite(line);
					pyflush();
				}

				setp(pbase(), epptr());
			}
			return 0;
		}

	public:

		PyOBytesBuf(pybind11::object pyostream, size_t buffer_size = 1024)
				: buf_size(buffer_size),
				  d_buffer(new char[buf_size]),
				  pywrite(pyostream.attr("write")),
				  pyflush(pyostream.attr("flush")) {
			setp(d_buffer.get(), d_buffer.get() + buf_size - 1);
		}

		PyOBytesBuf(PyOBytesBuf&&) = default;

		/// Sync before destroy
		~PyOBytesBuf() {
			sync();
		}
	};

	class PyIBytesBuf : public std::streambuf {
	private:
		using traits_type = std::streambuf::traits_type;

		const size_t buf_size;
		std::unique_ptr<char[]> d_buffer;
		pybind11::object pyread;

	public:
		/**
		 * Executed when the buffer is out of contents
		 */
		int underflow() {
			ssize_t len = 0;
			if (!traits_type::eq_int_type(*gptr(), traits_type::eof())) {
				// Not EOF yet, try to read more stuff from pyistream
				pybind11::bytes b = pyread(buf_size);
				len = pybind11::len(b);
				std::string incoming(b);
				const char * cstring = incoming.c_str();
				std::memcpy(d_buffer.get(), cstring, len);
			}
			setg(d_buffer.get(), d_buffer.get(), d_buffer.get() + len - 1);
			return len == 0 ? traits_type::eof() : traits_type::not_eof(*gptr());
		}

		PyIBytesBuf(pybind11::object pyistream, size_t buffer_size = 1024)
				: buf_size(buffer_size),
				  d_buffer(new char[buf_size]),
				  pyread(pyistream.attr("read")) {
			setg(d_buffer.get(), d_buffer.get() + buf_size - 1, d_buffer.get() + buf_size - 1);
		};

		PyIBytesBuf(PyIBytesBuf&&) = default;
	};
}


#endif //NBTP_PYBYTESBUF_H
