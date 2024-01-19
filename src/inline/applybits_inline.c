/*
 *  Copyright (c) 2009-2024, Peter Haag
 *  Copyright (c) 2004-2008, SWITCH - Teleinformatikdienste fuer Lehre und Forschung
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of the author nor the names of its contributors may be
 *     used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

static inline uint32_t ApplyV4NetMaskBits(uint32_t ip, uint32_t maskBits);

static inline uint32_t ApplyV4NetMaskBits(uint32_t ip, uint32_t maskBits) {
    uint32_t srcMask = 0xffffffff << (32 - maskBits);
    return (ip &= srcMask);
}  // End of ApplyV4NetMaskBits

static inline uint64_t *ApplyV6NetMaskBits(uint64_t *ip, uint32_t maskBits);

static inline uint64_t *ApplyV6NetMaskBits(uint64_t *ip, uint32_t maskBits) {
    static uint64_t net[2];
    uint64_t mask;
    if (maskBits > 64) {
        mask = 0xffffffffffffffffLL << (128 - maskBits);
        net[0] = ip[0];
        net[1] = ip[1] & mask;
    } else {
        mask = 0xffffffffffffffffLL << (64 - maskBits);
        net[0] = ip[0] & mask;
        net[1] = 0;
    }
    return net;

}  // End of ApplyV6NetMaskBits

static inline void ApplyNetMaskBits(recordHandle_t *recordHandle, int apply_netbits);

static inline void ApplyNetMaskBits(recordHandle_t *recordHandle, int apply_netbits) {
    EXipv4Flow_t *ipv4Flow = (EXipv4Flow_t *)recordHandle->extensionList[EXipv4FlowID];
    EXipv6Flow_t *ipv6Flow = (EXipv6Flow_t *)recordHandle->extensionList[EXipv6FlowID];
    EXflowMisc_t *flowMisc = (EXflowMisc_t *)recordHandle->extensionList[EXflowMiscID];

    uint32_t srcMask = 0;
    uint32_t dstMask = 0;
    if (flowMisc) {
        srcMask = flowMisc->srcMask;
        dstMask = flowMisc->dstMask;
    }

    if (ipv4Flow) {
        if (apply_netbits & 1) {
            uint32_t srcmask = 0xffffffff << (32 - srcMask);
            ipv4Flow->srcAddr &= srcmask;
        }
        if (apply_netbits & 2) {
            uint32_t dstmask = 0xffffffff << (32 - dstMask);
            ipv4Flow->dstAddr &= dstmask;
        }

    } else if (ipv6Flow) {
        if (apply_netbits & 1) {
            uint64_t mask;
            uint32_t mask_bits = srcMask;
            if (mask_bits > 64) {
                mask = 0xffffffffffffffffLL << (128 - mask_bits);
                ipv6Flow->srcAddr[1] &= mask;
            } else {
                mask = 0xffffffffffffffffLL << (64 - mask_bits);
                ipv6Flow->srcAddr[0] &= mask;
                ipv6Flow->srcAddr[1] = 0;
            }
        }
        if (apply_netbits & 2) {
            uint64_t mask;
            uint32_t mask_bits = dstMask;

            if (mask_bits > 64) {
                mask = 0xffffffffffffffffLL << (128 - mask_bits);
                ipv6Flow->dstAddr[1] &= mask;
            } else {
                mask = 0xffffffffffffffffLL << (64 - mask_bits);
                ipv6Flow->dstAddr[0] &= mask;
                ipv6Flow->dstAddr[1] = 0;
            }
        }
    }

}  // End of ApplyNetMaskBits

static inline void SetNetMaskBits(EXipv4Flow_t *EXipv4Flow, EXipv6Flow_t *EXipv6Flow, EXflowMisc_t *EXflowMisc, int apply_netbits);

static inline void SetNetMaskBits(EXipv4Flow_t *EXipv4Flow, EXipv6Flow_t *EXipv6Flow, EXflowMisc_t *EXflowMisc, int apply_netbits) {
    if (EXipv6Flow) {  // IPv6
        if (apply_netbits & 1) {
            uint64_t mask;
            uint32_t mask_bits = EXflowMisc->srcMask;
            if (mask_bits > 64) {
                mask = 0xffffffffffffffffLL << (128 - mask_bits);
                EXipv6Flow->srcAddr[1] &= mask;
            } else {
                mask = 0xffffffffffffffffLL << (64 - mask_bits);
                EXipv6Flow->srcAddr[0] &= mask;
                EXipv6Flow->srcAddr[1] = 0;
            }
        }
        if (apply_netbits & 2) {
            uint64_t mask;
            uint32_t mask_bits = EXflowMisc->dstMask;

            if (mask_bits > 64) {
                mask = 0xffffffffffffffffLL << (128 - mask_bits);
                EXipv6Flow->dstAddr[1] &= mask;
            } else {
                mask = 0xffffffffffffffffLL << (64 - mask_bits);
                EXipv6Flow->dstAddr[0] &= mask;
                EXipv6Flow->dstAddr[1] = 0;
            }
        }
    } else if (EXipv4Flow) {  // IPv4
        if (apply_netbits & 1) {
            uint32_t srcmask = 0xffffffff << (32 - EXflowMisc->srcMask);
            EXipv4Flow->srcAddr &= srcmask;
        }
        if (apply_netbits & 2) {
            uint32_t dstmask = 0xffffffff << (32 - EXflowMisc->dstMask);
            EXipv4Flow->dstAddr &= dstmask;
        }
    }

}  // End of SetNetMaskBits

static inline void ApplyAggrMask(master_record_t *record, master_record_t *mask) {
    uint64_t *r = (uint64_t *)record;
    uint64_t *m = (uint64_t *)mask;

    for (int i = INDEX_BASE; i < Offset_MR_LAST; i++) {
        r[i] &= m[i];
    }

}  // End of ApplyAggrMask
