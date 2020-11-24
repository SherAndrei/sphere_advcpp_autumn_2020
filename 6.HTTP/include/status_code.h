#ifndef HTTP_STATUS_CODE_H
#define HTTP_STATUS_CODE_H
#include <cstdint>

namespace http {

enum class StatusCode : uint16_t {
    UNKNOWN  = 0,

    Continue          = 100,
    SwitchingProtocol,
    Processing,

    OK = 200,
    Created,
    Accepted,
    NonAuthorativeInformation,
    NoContent,
    ResetContent,
    PartialContent,
    MultiStatus,
    AlreadyReported,
    IMUsed = 226,

    MultipleChoices  = 300,
    MovedPermanently,
    Found,
    SeeOther,
    NotModified,
    UseProxy,
    UNUSED,
    TemporaryRedirect,
    PermanentRedirect,

    BadRequest = 400,
    Unauthorized,
    PaymentRequired,
    Forbidden,
    NotFound,
    MethodNotAllowed,
    NotAcceptable,
    ProxyAuthenticationRequired,
    RequestTimeout,
    Conflict,
    Gone,
    LengthRequired,
    PreconditionFailed,
    PayloadTooLarge,
    URITooLong,
    UnsupportedMediaType,
    RequestedRangeNotSatisfiable,
    ExpectationFailed,
    ImATeapot,
    AuthenticationTimeout,
    MisdirectedRequest = 421,
    UnprocessableEntity,
    Locked,
    FailedDependency,
    UpgradeRequired = 426,
    PreconditionRequired = 428,
    TooManyRequests,
    RequestHeaderFieldsTooLarge     = 431,
    ConnectionClosedWithoutResponse = 444,
    RetryWith                       = 449,
    UnavailableForLegalReasons      = 451,
    ClientClosedRequest             = 499,

    InternalServerError = 500,
    NotImplemented,
    BadGateway,
    ServiceUnavailable,
    GatewayTimeout,
    HTTPVersionNotSupported,
    VariantAlsoNegotiates,
    InsufficientStorage,
    LoopDetected,
    BandwidthLimitExceeded,
    NotExtended,
    NetworkAuthenticationRequired,
    UnknownError = 520,
    WebServerIsDown,
    ConnectionTimedOut,
    OriginIsUnreachable,
    ATimeoutOccured,
    SSLHadnshakeFailed,
    NetworkConnectTimeoutError = 599
};

}  // namespace http

#endif  // HTTP_STATUS_CODE_H
