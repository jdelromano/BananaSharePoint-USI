#ifndef MICROSOFT_SECRETS_H
#define MICROSOFT_SECRETS_H

#include <QString>

namespace Microsoft_secrets {
const QString client_id  = "c4686722-538a-4ddf-ae75-e2f4480788be";
const QString auth_uri = "http://localhost:3000/";
const QString token_uri = "https://login.microsoftonline.com/687e1310-0790-4edc-8391-b528803559fc/oauth2/v2.0/token";
const QString client_secret = "sd.8Q~gVOre9L2YW.0SfnSIffV-xUY_1XmnfrblL";
const QString scope = "https://graph.microsoft.com/.default";
const QString redirect_uris = "http://localhost:3000/authdone";
const int port= 3000;
}
#endif // MICROSOFT_SECRETS_H
