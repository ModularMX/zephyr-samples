#!/usr/bin/env python3
"""
Generate TLS certificates for HTTPS server example.
Uses cryptography library - compatible with Python 3.12+
Equivalent to gen_ca_cert.sh and gen_server_cert.sh
"""

import os
from pathlib import Path
from datetime import datetime, timedelta, timezone

try:
    from cryptography import x509
    from cryptography.x509.oid import NameOID
    from cryptography.hazmat.primitives import hashes, serialization
    from cryptography.hazmat.primitives.asymmetric import ec
except ImportError:
    print("ERROR: cryptography library not installed")
    print("Install with: pip install cryptography")
    exit(1)

def generate_ca_certificate(cert_path, key_path):
    """Generate CA certificate and private key (equivalent to gen_ca_cert.sh)"""
    
    # Generate private key
    private_key = ec.generate_private_key(ec.SECP256R1())
    
    # Build certificate
    subject = issuer = x509.Name([
        x509.NameAttribute(NameOID.ORGANIZATION_NAME, "Zephyrproject"),
        x509.NameAttribute(NameOID.COMMON_NAME, "zephyr-ca"),
    ])
    
    cert = x509.CertificateBuilder().subject_name(
        subject
    ).issuer_name(
        issuer
    ).public_key(
        private_key.public_key()
    ).serial_number(
        x509.random_serial_number()
    ).not_valid_before(
        datetime.now(timezone.utc)
    ).not_valid_after(
        datetime.now(timezone.utc) + timedelta(days=36500)
    ).add_extension(
        x509.BasicConstraints(ca=True, path_length=None),
        critical=True,
    ).add_extension(
        x509.SubjectKeyIdentifier.from_public_key(private_key.public_key()),
        critical=False,
    ).sign(private_key, hashes.SHA256())
    
    # Save certificate in PEM
    with open(cert_path, "wb") as f:
        f.write(cert.public_bytes(serialization.Encoding.PEM))
    
    # Save private key in PEM
    with open(key_path, "wb") as f:
        f.write(private_key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.TraditionalOpenSSL,
            encryption_algorithm=serialization.NoEncryption()
        ))
    
    return cert, private_key

def generate_server_certificate(ca_cert, ca_key, cert_path, key_path):
    """Generate server certificate signed by CA (equivalent to gen_server_cert.sh)"""
    
    # Generate server private key
    server_key = ec.generate_private_key(ec.SECP256R1())
    
    # Build certificate request
    csr = x509.CertificateSigningRequestBuilder().subject_name(
        x509.Name([
            x509.NameAttribute(NameOID.ORGANIZATION_NAME, "Zephyrproject"),
            x509.NameAttribute(NameOID.COMMON_NAME, "zephyr"),
        ])
    ).sign(server_key, hashes.SHA256())
    
    # Sign with CA - with matching extensions from gen_server_cert.sh
    server_cert = x509.CertificateBuilder().subject_name(
        csr.subject
    ).issuer_name(
        ca_cert.issuer
    ).public_key(
        csr.public_key()
    ).serial_number(
        x509.random_serial_number()
    ).not_valid_before(
        datetime.now(timezone.utc)
    ).not_valid_after(
        datetime.now(timezone.utc) + timedelta(days=36500)
    ).add_extension(
        x509.SubjectKeyIdentifier.from_public_key(csr.public_key()),
        critical=False,
    ).add_extension(
        x509.AuthorityKeyIdentifier.from_issuer_public_key(ca_key.public_key()),
        critical=False,
    ).add_extension(
        x509.BasicConstraints(ca=False, path_length=None),
        critical=True,
    ).add_extension(
        x509.KeyUsage(
            digital_signature=True,
            content_commitment=False,
            key_encipherment=False,
            data_encipherment=False,
            key_agreement=False,
            key_cert_sign=False,
            crl_sign=False,
            encipher_only=False,
            decipher_only=False,
        ),
        critical=True,
    ).add_extension(
        x509.ExtendedKeyUsage([x509.oid.ExtendedKeyUsageOID.SERVER_AUTH]),
        critical=False,
    ).sign(ca_key, hashes.SHA256())
    
    # Save server certificate in PEM
    with open(cert_path, "wb") as f:
        f.write(server_cert.public_bytes(serialization.Encoding.PEM))
    
    # Save server private key in PEM
    with open(key_path, "wb") as f:
        f.write(server_key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.TraditionalOpenSSL,
            encryption_algorithm=serialization.NoEncryption()
        ))

def pem_to_der_cert(pem_path, der_path):
    """Convert PEM certificate to DER format"""
    with open(pem_path, "rb") as f:
        cert = x509.load_pem_x509_certificate(f.read())
    
    der_data = cert.public_bytes(serialization.Encoding.DER)
    with open(der_path, "wb") as f:
        f.write(der_data)

def pem_to_der_key(pem_path, der_path):
    """Convert PEM private key to DER format (EC format)"""
    with open(pem_path, "rb") as f:
        key = serialization.load_pem_private_key(f.read(), password=None)
    
    # Use EC format for private keys (same as openssl ec -outform der)
    from cryptography.hazmat.primitives.asymmetric import ec as ec_module
    if isinstance(key, ec_module.EllipticCurvePrivateKey):
        der_data = key.private_bytes(
            encoding=serialization.Encoding.DER,
            format=serialization.PrivateFormat.TraditionalOpenSSL,
            encryption_algorithm=serialization.NoEncryption()
        )
    else:
        # Fallback to PKCS8 for other key types
        der_data = key.private_bytes(
            encoding=serialization.Encoding.DER,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=serialization.NoEncryption()
        )
    
    with open(der_path, "wb") as f:
        f.write(der_data)

def main():
    script_dir = Path(__file__).parent
    
    # File paths
    ca_cert_pem = script_dir / "ca_cert.pem"
    ca_key_pem = script_dir / "ca_privkey.pem"
    ca_cert_der = script_dir / "ca_cert.der"
    server_cert_pem = script_dir / "server_cert.pem"
    server_key_pem = script_dir / "server_privkey.pem"
    server_cert_der = script_dir / "server_cert.der"
    server_key_der = script_dir / "server_privkey.der"
    
    # Generate CA certificate (gen_ca_cert.sh equivalent)
    if not ca_cert_pem.exists() or not ca_key_pem.exists():
        generate_ca_certificate(str(ca_cert_pem), str(ca_key_pem))
    
    # Load CA for signing server cert
    with open(ca_cert_pem, "rb") as f:
        ca_cert = x509.load_pem_x509_certificate(f.read())
    with open(ca_key_pem, "rb") as f:
        ca_key = serialization.load_pem_private_key(f.read(), password=None)
    
    # Generate server certificate (gen_server_cert.sh equivalent)
    generate_server_certificate(ca_cert, ca_key, str(server_cert_pem), str(server_key_pem))
    
    # Convert to DER format
    pem_to_der_cert(str(server_cert_pem), str(server_cert_der))
    pem_to_der_key(str(server_key_pem), str(server_key_der))
    
    # Also generate CA DER for completeness
    pem_to_der_cert(str(ca_cert_pem), str(ca_cert_der))

if __name__ == "__main__":
    main()
