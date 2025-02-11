import Link from "next/link"
import styled from "styled-components"
import Center from "@/components/Center";
import { useContext, useState } from "react";
import { CartContext } from "./CartContext";
import BarsIcon from "./icons/BarsIcon";
import { primary } from "@/lib/colors";


const StyledHeader = styled.header`
    background-color: ${primary}
`;

const Logo = styled(Link)`
    display: flex;
    align-items: center; 
    color: #fff;
    text-decoration: none;
    position: relative; 
    z-index: 3;
`;
const Wrapper = styled.div`
    display: flex;
    justify-content: space-between;
    padding: 20px 0;

`;

const StyledNav = styled.nav`
    ${props => props.mobileNavActive ?
         'display: block; padding: 50px 20px 20px;' : 
         'display: none; padding: 20px 20px 20px; align-items: right;'
    }
    gap: 15px;
    position: fixed;
    top: 0; 
    bottom: 0; 
    left: 0; 
    right: 0; 
    background-color: ${primary};
    @media screen and (min-width: 768px) { 
        display: flex;  
        position: static; 
    }
`;

const Image = styled.img`
    max-width: 40px; 
    max-height: 40px; 

`;

const NavLink = styled(Link)`
    display: block; 
    color: #fff;
    text-decoration: none; 
    @media screen and (min-width: 768px) {
        padding: 0;
    }
    padding: 10px 0;

`;

const NavButton = styled.button`
    background-color: transparent;
    width: 35px; 
    height: 35px; 
    border: 0; 
    color: white; 
    cursor: pointer; 
    position: relative; 
    z-index: 3; 
    @media screen and (min-width: 768px) {
        display: none; 
    }

`;

export default function Header() {
    const {cartProducts} = useContext(CartContext)
    const [mobileNavActive,setMobileNavActive] = useState(false)
    return (
        <StyledHeader>
            <Center>
                <Wrapper>
                    <Logo href={'/'}>
                        <b>TCTrader</b>
                        <Image src="/sax.png"/>
                    </Logo>
                    <StyledNav mobileNavActive={mobileNavActive}>
                        <NavLink href={'/'}>Home</NavLink>
                        <NavLink href={'/products'}>All Products</NavLink>
                        <NavLink href={'/cart'}>Cart ({cartProducts.length})</NavLink>
                    </StyledNav>
                    <NavButton onClick={() => setMobileNavActive(prev => !prev)}>
                        <BarsIcon />
                    </NavButton>
                </Wrapper>
            </Center>
        </StyledHeader>
    )
}