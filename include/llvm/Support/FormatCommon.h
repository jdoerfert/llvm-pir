//===- FormatAdapters.h - Formatters for common LLVM types -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_FORMATCOMMON_H
#define LLVM_SUPPORT_FORMATCOMMON_H

#include "llvm/ADT/SmallString.h"
#include "llvm/Support/FormatVariadicDetails.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
enum class AlignStyle { Left, Center, Right };

struct FmtAlign {
  detail::format_wrapper &Wrapper;
  AlignStyle Where;
  size_t Amount;

  FmtAlign(detail::format_wrapper &Wrapper, AlignStyle Where, size_t Amount)
      : Wrapper(Wrapper), Where(Where), Amount(Amount) {}

  void format(raw_ostream &S, StringRef Options) {
    // If we don't need to align, we can format straight into the underlying
    // stream.  Otherwise we have to go through an intermediate stream first
    // in order to calculate how long the output is so we can align it.
    // TODO: Make the format method return the number of bytes written, that
    // way we can also skip the intermediate stream for left-aligned output.
    if (Amount == 0) {
      Wrapper.format(S, Options);
      return;
    }
    SmallString<64> Item;
    raw_svector_ostream Stream(Item);

    Wrapper.format(Stream, Options);
    if (Amount <= Item.size()) {
      S << Item;
      return;
    }

    size_t PadAmount = Amount - Item.size();
    switch (Where) {
    case AlignStyle::Left:
      S << Item;
      S.indent(PadAmount);
      break;
    case AlignStyle::Center: {
      size_t X = PadAmount / 2;
      S.indent(X);
      S << Item;
      S.indent(PadAmount - X);
      break;
    }
    default:
      S.indent(PadAmount);
      S << Item;
      break;
    }
  }
};
}

#endif